 #include <SPI.h> //직렬 주변기기 신속통신 라이브러리
#include <EEPROM.h> //UID를 저장할 아두이노 내장 메모리 라이브러리
#include <MFRC522.h> //RFID 라이브러리
#include <Wire.h> // i2c 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h> // LCD 1602 I2C용 라이브러리
#include <SoftwareSerial.h> //시리얼 통신 라이브러리
#include "ESP8266.h"

#define SSID        "409LAB-2" //클라이언트 Wi-fi SSID
#define PASSWORD    "409bigdata" //클라이언트 Wi-fi Passwd
#define HOST_NAME   "192.168.0.9" //클라이언트로 접속할 Wi-fi의 IP
#define TCP_PORT   (80) //TCP 연결포트
#define TCP_NAME   "192.168.0.22" //TCP 연결할 APM 리눅스 서버의 IP
//아두이노와의 시리얼 통신을 위한 핀 번호 정의
#define BT_RXD 2 //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀 넘버  TX 2으로 선언
#define BT_TXD 3 //WIFI ESP wifi 모듈 시리얼 통신을 위한 핀 넘버  RX 3으로 선언
#define RELAY 4 //도어락 잠금장치를 조종할 릴레이 모듈의 핀 번호 4로 선언
#define wipeB 5 //EEPROM 레코드 리셋 버튼을 위한 핀 5
#define BUZZER 6 // BUZZER를 핀 번호 6으로 선언, 부저 제어
#define LED_R 7 // 빨간 LED를 핀 번호 7로 선언, 빨간색 LED
#define LED_G 8 // 초록 LED를 핀 번호 8으로 선언, 녹색 LED
#define RST_PIN 9 // RFID모듈 RST_PIN을 핀 번호 9로 선언, reset 핀
#define SS_PIN 10 // RFID모듈 SS_PIN을 핀 번호 10으로 선언, 데이터를 주고 받는 핀(SS = Slave Selector)

String temp;
uint8_t buffer[256] = {0}; //HTTP 패킷 버퍼
//uint32_t buffer[256] = {0}; //HTTP 패킷 버퍼
byte readCard[4];    // RFID 모듈에서 읽어온 UID를 저장한다
byte masterCard[4];   // RFID 마스터 번호를 저장한다
byte storedCard[4]; // EEPROM 메모리에서 읽어온 UID를 저장한다
boolean match = false;          // initialize card match to false
boolean programMode = false;  // initialize programming mode to false
boolean replaceMaster = false;
int successRead; //Variable integer to keep if we have Successful Read from Reader

LiquidCrystal_I2C lcd(0x3F, 16, 2); //1602디스플레이 접근 주소 : 0x3F or 0x27
MFRC522 mfrc(SS_PIN, RST_PIN); // MFR522를 이용하기 위해 mfrc객체 생성
SoftwareSerial ESP_wifi(BT_RXD, BT_TXD); //esp모듈 WIFI 객체를 선언한다
ESP8266 wifi(ESP_wifi); //ESP 와이파이 모듈이 아두이노와의 시리얼 통신을 연결해준다


////////////////////////////////////////셋업////////////////////////////////////////////
void setup() {                                                            
  pinMode(LED_G, OUTPUT); // 디지털 핀 8번 출력
  pinMode(LED_R, OUTPUT); // 디지털 핀 7번 출력
  pinMode(BUZZER, OUTPUT);// 디지털 핀 6번 출력
  pinMode(wipeB, INPUT_PULLUP);// 디지털 핀 5번 출력
  pinMode(RELAY, OUTPUT); 
  Serial.begin(9600); // 시리얼 통신 속도 9600
  SPI.begin(); // SPI 초기화 (하나의 master와 다수의 slave의 통신 방식)
  
  if (EEPROM.read(1) != 143) {
    lcd.init(); // LCD 화면 초기화
    lcd.setCursor(0, 0); // LCD 1번 칸, 1번 줄
    lcd.print("No Master Card Defiend");
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getID();
     } while (!successRead);
   
     for ( int j = 0; j < 4; j++ ) {        // 4번 루프
       EEPROM.write( 2 + j, readCard[j] );  // Write scanned PICC's UID to EEPROM, start from address 3
     }
     
    EEPROM.write(1, 143);                  // Write to EEPROM we defined Master Card.
    Serial.println(F("Master Card Defined"));
  }
  tone(BUZZER, 523, 100); //삐빅 소리
  Serial.println(F("-------------------"));
  Serial.println(F("Master Card's UID"));
  for ( int i = 0; i < 4; i++ ) {          // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2 + i);    // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }

  Serial.println("");
  Serial.println(F("-------------------"));
  
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
    
    if (wifi.enableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
   
  mfrc.PCD_Init(); //LCD모니터 Initiate
  lcd.init(); // LCD 화면 초기화
  lcd.backlight(); // 백라이트 켜기
  lcd.setCursor(0, 0); // LCD 1번 칸, 1번 줄
  lcd.print("Tag Searching");
  digitalWrite(LED_G, LOW);  // 8번 핀에 연결된 초록 LED OFF
  digitalWrite(LED_R, HIGH); // 7번 핀에 연결된 빨간 LED ON
}


////////////////////////////////////////루프////////////////////////////////////////////
void loop() {   
  do {
    successRead = getID(); //리더기에 태그가 인식된다면 successRead가 1, 그렇지 않으면 0
    if (digitalRead(wipeB) == HIGH) {
      Serial.println(F("Wipe Button Pressed"));
      Serial.println(F("Master Card will be Erased! in 5 seconds"));
      lcd.init();
      lcd.setCursor(0, 0); 
      lcd.print("Start Wiping after 5 secs");
      toneN(2);
      delay(5000);
      if (digitalRead(wipeB) == HIGH) {
        EEPROM.write(1, 0);                  //Magic Number 리셋
        Serial.println(F("Starting Wiping EEPROM"));
        for (int x = 0; x < EEPROM.length(); x = x + 1) {    //Loop end of EEPROM address
          if (EEPROM.read(x) == 0) {              //If EEPROM address 0
            // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
          }
          else {
            EEPROM.write(x, 0);       // if not write 0 to clear, it takes 3.3mS
          }
        }
      Serial.println(F("EEPROM Successfully Wiped"));
      Serial.println(F("Restart device to re-program Master Card"));
      lcd.init();
      lcd.setCursor(0, 0); 
      lcd.print("Wiped");
      while (1);
      }
    }
  } while(!successRead); //태그가 인식되지 않는다면 코드가 더 이상 진행되지 않음
  
  if (programMode) {
    if ( isMaster(readCard) ) { //마스터 카드가 인식된다면 프로그램 모드 종료
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      lcd.init();
      lcd.setCursor(0, 0); 
      lcd.print("MasterModeExit");
      delay(3000);
      lcd.init();
      lcd.setCursor(0, 0); 
      lcd.print("Tag searching");
      programMode = false;
      toneN(2);
      return;
    }
    else {
      if ( findID(readCard) ) { // 회원 태그가 인식되면 ROM에서 삭제한다
        Serial.println(F("I know this PICC, removing..."));
        deleteID(readCard);
        Serial.println("-----------------------------");
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
        lcd.init();
        lcd.setCursor(0, 0); 
        lcd.print("Delete  RFID"); delay(3000);
        lcd.init();
        lcd.setCursor(0, 0); 
        lcd.print("MasterMode");
      }
      else { // 회원 태그가 인식되면 ROM에 새로 추가한다
        Serial.println(F("I do not know this PICC, adding..."));
        writeID(readCard);
        Serial.println(F("-----------------------------"));
        Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
        lcd.init();
        lcd.setCursor(0, 0); 
        lcd.print("Add RFID"); delay(3000);
        lcd.init();
        lcd.setCursor(0, 0); 
        lcd.print("MasterMode");
      }
    }
  }
  else {
    if ( isMaster(readCard)) { // 마스터 카드가 인식된다면 프로그램 모드로 전환한다
      programMode = true;
      tone(BUZZER, 523, 100); //삐빅 소리
      Serial.println(F("Hello Master - Entered Program Mode"));
      int count = EEPROM.read(0);   // EEPROM의 첫번째 바이트 데이터를 읽어온다
      Serial.print(F("I have "));     // ID 레코드의 수를 출력
      Serial.print(count);
      Serial.print(F(" record(s) on EEPROM"));
      Serial.println("");
      Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
      Serial.println(F("Scan Master Card again to Exit Program Mode"));
      Serial.println(F("-----------------------------"));
      lcd.init();
      lcd.setCursor(0, 0); 
      lcd.print("MasterMode");
    }
    else {
      if ( findID(readCard) ) { //인식된 태그가 회원이라면 승인한다
        Serial.println(F("Welcome, You shall pass"));
        granted(300); // Open the door lock for 300 ms
      }
      else {
        Serial.println(F("You shall not pass"));
        denied(); // 회원이 아닌 태그가 인식되면 거부한다
      }
    }
  }
}
///////////////////////////////RFID & EEPROM 기능 함수////////////////////////////////////////
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
    Serial.print(readCard[i], HEX);
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

void readID( int number ) {
  int start = (number * 4 ) + 2;     // Figure out starting position
  for ( int i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

void writeID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);     // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6;  // Figure out where the next slot starts
    num++;                // Increment the counter by one
    EEPROM.write( 0, num );     // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {   // Loop 4 times
      EEPROM.write( start + j, a[j] );  // Write the array values to EEPROM in the right position
    }
    Serial.println(F("Succesfully added ID record to EEPROM"));
  }
  else {
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  tone(BUZZER, 523, 100); //삐빅 소리
}

void deleteID( byte a[] ) {
  if ( !findID( a ) ) {     // Before we delete from the EEPROM, check to see if we have this card!
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else {
    int num = EEPROM.read(0);   // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot;       // Figure out the slot number of the card
    int start;      // = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping;    // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a );   // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;      // Decrement the counter by one
    EEPROM.write( 0, num );   // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) {         // Loop the card shift times
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 4; k++ ) {         // Shifting loop
      EEPROM.write( start + j + k, 0);
    }
    Serial.println(F("Succesfully removed ID record from EEPROM"));
  }
  toneN(3);
}

int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // Stop looking we found it
    }
  }
}

boolean findID( byte find[] ) {
  int count = EEPROM.read(0);      // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {    // Loop once for each EEPROM entry
    readID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;  // Stop looking we found it
    }
    else {    // If not, return false
    }
  }
  return false;
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
  lcd.setCursor(0, 1); 
  for (byte i = 0; i < 4; i ++) { // 태그의 ID를 출력
    lcd.print(mfrc.uid.uidByte[i]);
    lcd.print(" "); // id 사이의 간격 출력
  }
  digitalWrite(RELAY, HIGH); 
  toneN(1);
  delay(setDelay);
  lcd.init(); 
  lcd.setCursor(0, 0);
  lcd.print("Door Locked");
  digitalWrite(RELAY, LOW); 
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

void toneN(int n)
{
  for(int i = 0; i < n; i++) {
    tone(BUZZER, 523, 100); //삐빅 소리
    digitalWrite(LED_R, HIGH);
    delay(200);
    digitalWrite(LED_R, LOW);
  }
}

