#include <SPI.h> //네트워크 통신 라이브러리
#include <MFRC522.h> //RFID 라이브러리
#include <Wire.h> // i2c 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> // LCD 1602 I2C용 라이브러리
#include <SoftwareSerial.h> //ESP모듈 조작 라이브러리
#include "ESP8266.h"

#define SSID        "409 print"
#define PASSWORD    "409bigdata"
#define HOST_NAME   "192.168.0.9"
#define HOST_PORT   (80)
#define BT_RXD 2 //WIFI ESP모듈 wifi TX 2으로 선언
#define BT_TXD 3 //WIFI ESP모듈 wifi RX 3으로 선언
int Relay = 4; //도어락 잠금장치를 조종할 릴레이 모듈의 핀 번호
int buzzer = 6; // 변수 buzzer를 6으로 선언, 부저 제어
int LED_R = 7; // 변수 LED_R을 4로 선언, 빨간색 LED
int LED_G = 8; // 변수 LED_G를 8으로 선언, 녹색 LED
int RST_PIN = 9; // 변수 RST_PIN을 9로 선언, reset 핀
int SS_PIN = 10; // 변수 SS_PIN을 10으로 선언, 데이터를 주고 받는 핀(SS = Slave Selector)

SoftwareSerial ESP_wifi(BT_RXD, BT_TXD); //esp모듈 WIFI 객체를 선언한다
LiquidCrystal_I2C lcd(0x3F, 16, 2); //1602디스플레이 접근 주소 : 0x3F or 0x27
MFRC522 mfrc(SS_PIN, RST_PIN); // MFR522를 이용하기 위해 mfrc객체 생성
ESP8266 wifi(ESP_wifi);

void setup() {                                                            //셋업
  pinMode(LED_G, OUTPUT); // 디지털 핀 8번을 출력으로 설정
  pinMode(LED_R, OUTPUT); // 디지털 핀 7번을 출력으로 설정
  pinMode(5, OUTPUT); //디지털 핀 5번을 출력으로 설정
  pinMode(buzzer, OUTPUT); // 디지털 핀 6번을 출력으로 설정
  pinMode(Relay, OUTPUT); 
  Serial.begin(9600); // 시리얼 통신 속도 9600
  SPI.begin(); // SPI 초기화 (하나의 master와 다수의 slave의 통신 방식)
  
  ESP_wifi.begin(9600); 
  ESP_wifi.setTimeout(5000);
  if (wifi.joinAP(SSID, PASSWORD)) { //Wi-fi에 접속한다
        Serial.print("Join AP success\r\n");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.print("Join AP failure\r\n");
    }

    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }

    if(wifi.createTCP("192.168.0.9", 80))
      Serial.print("TCP ok\r\n");
    else
      Serial.print("TCP err\r\n");
    
    
    Serial.print("setup end\r\n");  //네트워크 셋업 종료

  mfrc.PCD_Init(); //LCD모니터 Initiate
  lcd.init(); // LCD 화면 초기화
  lcd.backlight(); // 백라이트 켜기
  lcd.setCursor(0, 0); // LCD 1번 칸, 1번 줄
  lcd.print("Door Locked");
  digitalWrite(LED_G, LOW);  // 8번 핀에 연결된 초록 LED OFF
  digitalWrite(LED_R, HIGH); // 7번 핀에 연결된 빨간 LED ON
}

void loop() {                                                             //루프
  if (Serial.available()){
    ESP_wifi.write(Serial.read()); 
  } 
  
  if (ESP_wifi.available()) { 
    Serial.write(ESP_wifi.read()); 
  }
  
  // 태그 접촉이 없음 혹은 ID가 읽히지 않음
  if ( !mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) {
    //delay(100); // 0.1초의 딜레이
    return;
  }
  /*
   * if(mfrc.uid.uidByte[0] && mfrc.uid.uidByte[1] && mfrc.uid.uidByte[2] && mfrc.uid.uidByte[3]) 
   * {
   * rfid 데이터를 서버로 전송한다. 
   * (rfid 데이터는 
   * 데이터베이스 서버 안에서 해당 
   * rfid 번호 존재 조회(탐색 알고리즘)
   * }
   */
    //허용된 NFC 센서가 인식되었을 때
  if (mfrc.uid.uidByte[0] == 93 && mfrc.uid.uidByte[1] == 196 && mfrc.uid.uidByte[2] == 36 && mfrc.uid.uidByte[3] == 217) 
    { // 지정된 카드의 고유 ID
    DoorControl(true);
    } //
    else 
    { // 다른 태그의 ID일 경우
    DoorControl(false);
    }
    Serial.println("loop end");
}

void DoorControl(bool control) { //control 변수의 true, false를 받아 잠금장치를 조작한다
  if(control == true){ //잠금장치를 해제해 문을 개방
    lcd.init(); // LCD 초기화
    lcd.setCursor(0, 0); // LCD 1번 칸, 1번 줄
    lcd.print("Door Open!");
    
    lcd.setCursor(0, 1); // LCD 1번 칸, 2번 줄
    for (byte i = 0; i < 4; i ++) { // 태그의 ID출력을 ID사이즈 4 까지 반복
      lcd.print(mfrc.uid.uidByte[i]); // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
      lcd.print(" "); // id 사이의 간격 출력
    }
    
    digitalWrite(Relay, HIGH); 
    digitalWrite(LED_G, HIGH);  // 8번 핀에 연결된 초록 LED ON
    digitalWrite(LED_R, LOW); // 7번 핀에 연결된 빨간 LED OFF
    
    tone(buzzer, 523 , 100);
    delay(1000);

    int i = 3;

    while(i>0){
      lcd.setCursor(0, 0);
      lcd.print("Door Open! ");
      lcd.print(i);
      --i;
      if(i>0)
        delay(1000);
    }
    
    lcd.init(); // LCD 초기화
    lcd.setCursor(0, 0); // 1번 칸, 1번 줄
    lcd.print("Door Locked");
    digitalWrite(Relay, LOW); 
    digitalWrite(LED_G, LOW);  // 8번 핀에 연결된 초록 LED OFF
    digitalWrite(LED_R, HIGH); // 7번 핀에 연결된 빨간 LED ON

    tone(buzzer, 523, 100); //삐빅 소리
    delay(300);
    tone(buzzer, 523, 100);
    delay(300);
    }
    else //잠금장치를 On하여 문을 잠금
    {
      digitalWrite(Relay, LOW); 
      digitalWrite(LED_G, LOW);  // 8번 핀에 연결된 초록 LED OFF
      digitalWrite(LED_R, HIGH); // 7번 핀에 연결된 빨간 LED ON
      lcd.init(); // LCD 초기화
      lcd.setCursor(0, 0); // 1번 칸, 1번 줄
      lcd.print("Invalid ID");
      lcd.setCursor(0, 1); // 1번 칸, 2번 줄
      for (byte i = 0; i < 4; i ++) { // 태그의 ID출력을 ID사이즈 4 까지 반복
        lcd.print(mfrc.uid.uidByte[i]); // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
        lcd.print(" "); // id 사이의 간격 출력
      }
      tone(buzzer, 523, 100); //삐빅 소리
      delay(300);
      tone(buzzer, 523, 100);
      delay(300);
    }
}

