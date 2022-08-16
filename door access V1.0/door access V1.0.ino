#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <Servo.h>
#include <EEPROM.h>

#define RST_PIN         9          
#define SS_PIN          10 //RC522卡上的SDA

MFRC522 mfrc522;   // 建立MFRC522實體
Servo myservo;

char *reference;

byte uid[][7]={
  {0x04, 0x69, 0x7E, 0xF2, 0xC6, 0x69, 0x80},
  {0xA5, 0x7F, 0x75, 0x42},
  {0x72, 0x73, 0x92, 0x0A},
  {0xDE, 0xAD, 0xBE, 0xEF}};

byte EEPROMuid[102][10];
byte nonWrite[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int admintimes = -10000;
int cardcount = 0;

void setup()
{
  Serial.begin(9600);   
  myservo.attach(2);
  myservo.write(135);
  
  SPI.begin();
  
  mfrc522.PCD_Init(SS_PIN, RST_PIN); // 初始化MFRC522卡
  Serial.print(F("Reader "));
  Serial.print(F(": "));
  mfrc522.PCD_DumpVersionToSerial(); // 顯示讀卡設備的版本
  bool nocarduid = true;
  while(nocarduid){
    int cardUid[10];
    for(int j = 0; j < 10; j++){
      int address = (cardcount*10) + j;
      cardUid[j] = EEPROM.read(address);
      Serial.print(address);
      Serial.print("    ");
      Serial.println(cardUid[j]);
    }
    if(cardUid[0] == nonWrite[0]){
      nocarduid = 0;
    }
    else{
      for(int j = 0; j < 10; j++){
        EEPROMuid[cardcount][j] = cardUid[j];
      }
    }
    cardcount++;
  }
  Serial.print("Start read");
}

void loop() {
  //Serial.print("reading...");
  // 檢查是不是偵測到新的卡
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // 顯示卡片的UID
      Serial.print(F("Card UID:"));
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size); // 顯示卡片的UID
      Serial.println();
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));  //顯示卡片的類型
      bool they_match = false;
      //把取得的UID，拿來比對我們指定好的UID
      for ( int j = 0; j < 5 ; j++) {
        int match = true;
        for ( int i = 0; i < 4; i++ ) { // 卡片UID為4段，分別做比對
          if ( uid[j][i] != mfrc522.uid.uidByte[i] ) { 
            match = false; // 如果任何一個比對不正確，they_match就為false，然後就結束比對
            break; 
          }
        }
        if(match){
          admintimes = millis();
          they_match = true;
        }
      }
      for ( int j = 0; j < cardcount ; j++) {
        int match = true;
        for ( int i = 0; i < 4; i++ ) { // 卡片UID為4段，分別做比對
          if ( EEPROMuid[j][i] != mfrc522.uid.uidByte[i] ) { 
            match = false; // 如果任何一個比對不正確，they_match就為false，然後就結束比對
            break; 
          }
        }
        if(match){
          they_match = true;
        }
      }
      
      //在監控視窗中顯示比對的結果
      if(they_match){
        Serial.print(F("Access Granted!"));
        if((millis()-admintimes) < 1000){
          Serial.print(F("admin-card"));
        }
        myservo.write(45);
        delay(2500);
        myservo.write(135);
      }else{
        if((millis()-admintimes) < 5000){
          Serial.print(F("card update"));
          for(int j = 0; j < 10; j++){
            for(int k = 0; k < 10; k++){
              int address = ((cardcount-1)*10) + k;
              EEPROM.write(address, mfrc522.uid.uidByte[k]);
            }
          }
          cardcount++;
        }
        else{
          Serial.print(F("Access Denied!"));
        }
      }
      mfrc522.PICC_HaltA();  // 卡片進入停止模式
    }
}

/**
 * 這個副程式把讀取到的UID，用16進位顯示出來
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
