#include <SPI.h> //직렬 주변기기 신속통신 라이브러리
#include <EEPROM.h> //UID를 저장할 아두이노 내장 메모리 라이브러리
#include <MFRC522.h> //RFID 라이브러리
#include <Wire.h> // i2c 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> // LCD 1602 I2C용 라이브러리
#include <SoftwareSerial.h> //시리얼 통신 라이브러리
#include "ESP8266.h"

#define SSID        "" //클라이언트 Wi-fi SSID
#define PASSWORD    "" //클라이언트 Wi-fi Passwd
#define HOST_NAME   "192.168.0.9" //접속할 Wi-fi의 IP
#define TCP_PORT   (80) //TCP 연결포트
#define TCP_NAME   "192.168.0.22" //클라이언트로 TCP 연결할 APM 리눅스 서버의 IP
//아두이노와의 시리얼 통신을 위한 핀 번호 정의
#define BT_RXD 2 //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀 넘버  TX 2으로 선언
#define BT_TXD 3 //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀 넘버  RX 3으로 선언
#define RELAY 4 //도어락 잠금장치를 조종할 릴레이 모듈의 핀 번호 4로 선언
#define BUZZER 6 // BUZZER를 핀 번호 6으로 선언, 부저 제어
#define LED_R 7 // 빨간 LED를 핀 번호 7로 선언, 빨간색 LED
#define LED_G 8 // 초록 LED를 핀 번호 8으로 선언, 녹색 LED
#define RST_PIN 9 // RFID모듈 RST_PIN을 핀 번호 9로 선언, reset 핀
#define SS_PIN 10 // RFID모듈 SS_PIN을 핀 번호 10으로 선언, 데이터를 주고 받는 핀(SS = Slave Selector)

char nfc_temp[16] = {0}; //NFC 번호 판별을 위한 임시 배열
String temp;
uint8_t buffer[256] = {0}; //HTTP 패킷을 저장하기 위한 문자열 버퍼 256바이트
byte readCard[4];    // RFID 모듈에서 읽어온 UID를 저장한다
byte masterCard[4] = {87, 230, 85, 10};   // RFID 마스터 번호를 저장한다
byte storedCard[4]; // EEPROM 메모리에서 읽어온 UID를 저장한다

LiquidCrystal_I2C lcd(0x3F, 16, 2); //1602디스플레이 접근 주소 : 0x3F or 0x27
MFRC522 mfrc(SS_PIN, RST_PIN); // MFR522를 이용하기 위해 mfrc객체 생성

SoftwareSerial ESP_wifi(BT_RXD, BT_TXD); //esp모듈 WIFI 객체를 선언한다
ESP8266 wifi(ESP_wifi); //ESP 와이파이 모듈이 아두이노와의 시리얼 통신을 연결해준다

void setup() {                                                            // 아두이노 셋업
  pinMode(LED_G, OUTPUT); // 디지털 핀 8번 출력
  pinMode(LED_R, OUTPUT); // 디지털 핀 7번 출력
  pinMode(5, OUTPUT);     // 디지털 핀 5번 출력
  pinMode(BUZZER, OUTPUT);// 디지털 핀 6번 출력
  pinMode(RELAY, OUTPUT); 
  Serial.begin(9600); // 시리얼 통신 속도 9600
  SPI.begin(); // SPI 초기화 (하나의 master와 다수의 slave의 통신 방식)
  
  ESP_wifi.begin(9600); 
  ESP_wifi.setTimeout(5000);
  if (wifi.joinAP(SSID, PASSWORD)) { //Wi-fi에 접속한다
        Serial.print("Join AP success\r\n");
        Serial.println(wifi.getIPStatus().c_str());
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

  TCP_Connect(TCP_NAME, TCP_PORT);
  Serial.print("setup end\r\n");  //네트워크 셋업 종료
  char *hello = TCP_Packet_Char("setup_ok");       //TCP연결 "setup_ok" 메시지를 
  wifi.send((const uint8_t*)hello, strlen(hello)); //APM서버 데이터베이스에 전송
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
  
    //NFC 모듈에 태그가 인식되었을 때 93 196 36 217
  if (mfrc.uid.uidByte[0] && mfrc.uid.uidByte[1] && mfrc.uid.uidByte[2] && mfrc.uid.uidByte[3]) 
    { // 지정된 카드의 고유 ID
      DoorControl(true);
      delay(1000);
    }
    else
    {
      DoorControl(false);
    }
    Serial.println("Loop end");
}

void DoorControl(bool control) { //control 변수의 true, false를 받아 문의 잠금장치를 조작한다
  if(control == true){ //잠금장치를 해제해 문을 개방
    lcd.init(); // LCD 초기화
    lcd.setCursor(0, 0); // LCD 1번 칸, 1번 줄
    lcd.print("Door Open!");
    lcd.setCursor(0, 1); // LCD 1번 칸, 2번 줄
    for (byte i = 0; i < 4; i ++) { // 태그의 ID출력을 ID사이즈 4 까지 반복
      lcd.print(mfrc.uid.uidByte[i]); // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
      lcd.print(" "); // id 사이의 간격 출력
    }
    
    digitalWrite(RELAY, HIGH); 
    digitalWrite(LED_G, HIGH);  // 8번 핀에 연결된 초록 LED ON
    digitalWrite(LED_R, LOW); // 7번 핀에 연결된 빨간 LED OFF
    tone(BUZZER, 523 , 100);
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
    
    digitalWrite(RELAY, LOW); 
    digitalWrite(LED_G, LOW);  // 8번 핀에 연결된 초록 LED OFF
    digitalWrite(LED_R, HIGH); // 7번 핀에 연결된 빨간 LED ON
    tone(BUZZER, 523, 100); //삐빅 소리
    delay(300);
    tone(BUZZER, 523, 100);
    delay(300);
    }
    else //잠금장치를 On하여 문을 잠금
    {
      digitalWrite(RELAY, LOW); 
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
      tone(BUZZER, 523, 100); //삐빅 소리
      delay(300);
      tone(BUZZER, 523, 100);
      delay(300);
    }
}

void TCP_Connect(char* ip, int port) //리눅스apm 서버로 tcp연결 Setup. http 통신을 위함
{
   if(wifi.createTCP(ip, port)) 
      Serial.print("TCP ok\r\n");
    else
      Serial.print("TCP err\r\n");
}

void Release_TCP() {
  if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
}

char* TCP_Packet_Char(char *arg) //HTTP패킷 전송 함수(Character 형)
{
  temp = String(String("GET /addtest.php?nfc=") + arg + 
         String("\r\nHost: 192.168.0.22\r\nConnection: close\r\n\r\n"));
  temp.toCharArray(buffer, temp.length());
  return buffer;
}

char* TCP_Packet_Byte(byte arg1, byte arg2, byte arg3, byte arg4) //HTTP패킷 전송 함수(Byte 형)
{
  temp = String(String("GET /doorlock409/addtest.php?nfc=") + 
                        arg1 + "_" + 
                        arg2 + "_" + 
                        arg3 + "_" + 
                        arg4 + 
                String(" \r\nHost: 192.168.0.22\r\nConnection: close\r\n\r\n"));
  temp.toCharArray(buffer, temp.length());
  return buffer;
}

void TCP_NFC() 
{
  char *hello = "GET /doorlock409/fetch_reg_nfc.php HTTP/1.1\r\nHost: 192.168.0.22\r\nConnection: close\r\n\r\n";
    wifi.send((const uint8_t*)hello, strlen(hello));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
      }
}



