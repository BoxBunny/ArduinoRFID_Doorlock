#include <SPI.h> //직렬 주변기기 통신 라이브러리
#include <MFRC522.h> //RFID 라이브러리
#include <Wire.h> // i2c 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> // LCD 1602 I2C용 라이브러리
#include <SoftwareSerial.h> //시리얼 통신 라이브러리
#include <Servo.h>
#include "ESP8266.h"

#define SSID        "409LAB-2" //클라이언트 Wi-fi SSID
#define PASSWORD    "409bigdata" //클라이언트 Wi-fi Passwd
#define TCP_PORT   (80) //TCP 연결포트
#define TCP_NAME   "192.168.0.22" //TCP 연결할 APM 리눅스 서버의 IP
//아두이노와의 시리얼 통신을 위한 핀
#define ESP_RX 0 //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀
#define ESP_TX 1  //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀
#define SERVO 4 //도어락 잠금장치를 조종할 Servo모터의 핀
#define BUZZER 6 // BUZZER를 핀 번호 6으로 선언, 부저 제어
#define LED_R 7 // 빨간 LED를 핀 번호 선언, 빨간색 LED
#define LED_G 8 // 초록 LED를 핀 번호 선언, 녹색 LED
#define RST_PIN 9 // RFID모듈 RST_PIN을 핀 번호 선언, reset 핀
#define SS_PIN 10 // RFID모듈 SS_PIN을 핀 번호 선언, 데이터를 주고 받는 핀(SS = Slave Selector)

String temp;
uint8_t buffer[256] = {0}; //HTTP 패킷 버퍼
byte readCard[4];    // RFID 모듈에서 읽어온 UID를 저장한다
byte masterCard[4] = {87, 230, 85, 10};   // RFID 마스터 번호를 저장한다
byte user[4] = {93, 196, 36, 217};
boolean match = false;          // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
int successRead; //Variable integer to keep if we have Successful Read from Reader
int servo_pos = 0;

Servo DoorServo;
LiquidCrystal_I2C lcd(0x3F, 16, 2); //1602디스플레이 접근 주소 : 0x3F or 0x27
MFRC522 mfrc(SS_PIN, RST_PIN); // MFR522를 이용하기 위해 mfrc객체 생성
SoftwareSerial ESP_wifi(ESP_TX, ESP_RX); //esp모듈 WIFI 객체를 선언한다
ESP8266 wifi(ESP_wifi); //ESP 와이파이 객체


////////////////////////////////////////셋업////////////////////////////////////////////
void setup() {                                                            
  pinMode(LED_G, OUTPUT); // 디지털 핀 8번 출력
  pinMode(LED_R, OUTPUT); // 디지털 핀 7번 출력
  pinMode(BUZZER, OUTPUT);// 디지털 핀 6번 출력
  pinMode(SERVO, OUTPUT); 
  SPI.begin(); // SPI 초기화 (하나의 master와 다수의 slave의 통신 방식)
  Serial.begin(9600); // 시리얼 통신 속도 9600
  ESP_wifi.begin(9600); 
  ESP_wifi.setTimeout(5000);
  DoorServo.attach(SERVO);
  mfrc.PCD_Init(); //LCD모니터 Initiate
  lcd.backlight(); // 백라이트 켜기
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Tag Searching");
  digitalWrite(LED_G, LOW); 
  digitalWrite(LED_R, HIGH);
  for (servo_pos = 0; servo_pos <= 180; servo_pos++) {
    DoorServo.write(servo_pos);
    delay(2);
  }

  if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }
  
  if (wifi.joinAP(SSID, PASSWORD)) { //Wi-fi에 접속한다
        Serial.print("Join AP success\r\n");
        Serial.println(wifi.getIPStatus().c_str());
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());
        lcd.init();
        lcd.setCursor(0, 0);
        lcd.print("joinAP success");       
    } else {
        Serial.print("Join AP failure\r\n");
        lcd.init();
        lcd.setCursor(0, 0);
        lcd.print("joinAP failed");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
        TCP_Connect(TCP_NAME, TCP_PORT);

        String packet = "GET ";
        packet = packet.concat("/addtest.php?nfc=setup_ok\r\nHost: ");
        packet = packet.concat(TCP_NAME);
        packet = packet.concat("\r\nConnection: close\r\n\r\n");
        char hello[] = {};
        packet.toCharArray(hello, packet.length());
        wifi.send((const uint8_t*)hello, strlen(hello)); //APM서버 데이터베이스에 전송
  
        uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
        if (len > 0) {
            Serial.print("Received:[");
            for(uint32_t i = 0; i < len; i++) {
              Serial.print((char)buffer[i]);
            }
            Serial.print("]\r\n");
        }
    } else {
        Serial.print("single err\r\n");
    }
  
  Serial.print("setup end\r\n");  //네트워크 셋업 종료
}


////////////////////////////////////////루프////////////////////////////////////////////
void loop() {   
  if (Serial.available()){
    ESP_wifi.write(Serial.read()); 
  } 
  if (ESP_wifi.available()) { 
    Serial.write(ESP_wifi.read()); 
  }
  
  do {
    successRead = getID(); //리더기에 태그가 인식된다면 successRead가 1, 그렇지 않으면 0
  } while(!successRead); //태그가 인식되지 않는다면 코드가 더 이상 진행되지 않음
  
  if (programMode) {
    if ( isMaster(readCard) ) { //설정모드 종료
      programMode = false;
      lcd.init();
      lcd.setCursor(0, 0);
      lcd.print("Tag Searching");
      toneN(2);
      return;
    }
    else { //설정모드 입력
        writeID(readCard);
    }
  }
  else {
    if ( isMaster(readCard)) {    // 설정모드 진입
      programMode = true;
      lcd.init();
      lcd.setCursor(0, 0);
      lcd.print("MasterMode");
      toneN(2);
    }
    else { //문 열기닫기 작동
      Control_door(readCard);
    }
  }
}
/////////////////////////////////////네트워크 함수////////////////////////////////////////
void TCP_Connect(char* ip, int port) //리눅스apm 서버로 tcp연결 Setup. http 통신을 위함
{
   if(wifi.createTCP(ip, port)) 
      Serial.print("TCP ok\r\n");
    else
      Serial.print("TCP err\r\n");
}

void Release_TCP() { //TCP연결 종료
  if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
}

///////////////////////////////RFID 기능 함수////////////////////////////////////////
int getID() {
  if ( ! mfrc.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  Serial.println(F("Scanned PICC's UID:"));
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc.uid.uidByte[i];
    Serial.print(readCard[i]);
  }
  Serial.println("");
  mfrc.PICC_HaltA(); // Stop reading
  return 1;
}

boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL )       // 배열이 있다는 것을 증명한다
    match = true;       // 처음이 일치한다는 것을 추측한다
  for ( int k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] )     // 두 nfc 배열을 비교한다. 하나라도 맞지 않으면 false 반환
      match = false;
  }
  if ( match ) {      // Check to see if if match is still true
    return true;      // Return true
  }
  else  {
    return false;       // Return false
  }
}

void Control_door( byte a[] ) {
  int i = 0;
  for ( int k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != user[k] )     // 두 nfc 배열을 비교한다. 하나라도 맞지 않으면 false 반환
      i--;
  }

  if(i==0)
    granted(2000);
  /*
  String NFC = "";
  for (int i = 0; i < 4; i++) {  //
    NFC + String(a[i]) + " ";
  }

  String packet = "GET /doorlock409/fetch_reg_nfc.php?nfc=";
        packet = packet.concat(NFC);
        packet = packet.concat("\r\nHost: ");
        packet = packet.concat(TCP_NAME);
        packet = packet.concat("\r\nConnection: close\r\n\r\n");
        char hello[] = {};
        packet.toCharArray(hello, packet.length());
        wifi.send((const uint8_t*)hello, strlen(hello)); //APM서버 데이터베이스에 전송
  
        uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
        if (len > 0) {
            Serial.print("Received:[");
            for(uint32_t i = 0; i < len; i++) {
              Serial.print((char)buffer[i]);
            }
            Serial.print("]\r\n");
        }
  String dooropen(len);
  if(dooropen.indexOf("DOOR OPEN", 0)) {
    granted (2000);
  }
  else {
    denied();
  }
  */
}

void writeID( byte a[] ) {
  lcd.init();
  lcd.setCursor(0, 0); 
  lcd.print("Sending...");
  String NFC = "";
  for (int i = 0; i < 4; i++) {  //
    NFC + String(a[i]) + " ";
  }

  Serial.println(NFC);

  String packet = "GET /doorlock409/nfc_add.php?nfc=";
        packet = packet.concat(NFC);
        packet = packet.concat("\r\nHost: ");
        packet = packet.concat(TCP_NAME);
        packet = packet.concat("\r\nConnection: close\r\n\r\n");
        char hello[] = {};
        packet.toCharArray(hello, packet.length());
        wifi.send((const uint8_t*)hello, strlen(hello)); //APM서버 데이터베이스에 전송
  
        uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
        if (len > 0) {
            Serial.print("Received:[");
            for(uint32_t i = 0; i < len; i++) {
              Serial.print((char)buffer[i]);
            }
            Serial.print("]\r\n");
        }
}

boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}

void granted (int setDelay) { //허용된 태그 인식시 동작
  lcd.init();
  lcd.setCursor(0, 0); 
  lcd.print("Door Open!");

  for (servo_pos = 180; servo_pos >= 0; servo_pos--) {
    DoorServo.write(servo_pos);
    delay(2);
  }
  toneN(1);
  delay(setDelay);
  lcd.init(); 
  lcd.setCursor(0, 0);
  lcd.print("Door Locked");
  for (servo_pos = 0; servo_pos <= 180; servo_pos++) {
    DoorServo.write(servo_pos);
    delay(2);
  }
  toneN(2);
}

void denied() { //태그 거부 동작
  digitalWrite(LED_R, HIGH);
  lcd.init(); // LCD 초기화
  lcd.setCursor(0, 0); // 1번 칸, 1번 줄
  lcd.print("Invalid ID");
  lcd.setCursor(0, 1); // 1번 칸, 2번 줄
  for (byte i = 0; i < 4; i ++) { // 태그의 ID출력을 ID사이즈 4 까지 반복
    lcd.print(mfrc.uid.uidByte[i]); // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
    lcd.print(" "); // id 사이의 간격 출력   
  }
  toneN(3);
}

void toneN(int n){

  for(int i = 0; i < n; i++) {
    tone(BUZZER, 523, 100); //삐빅 소리
    digitalWrite(LED_R, HIGH);
    delay(200);
    digitalWrite(LED_R, LOW);
  }
}

