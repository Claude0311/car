#define Rx 5
#define Tx 4
#include <SoftwareSerial.h>
//#include <iomanip>
SoftwareSerial BTSerial(Rx, Tx);

#include <SPI.h>
#include <MFRC522.h> // 引用程式庫
#define RST A5 // 讀卡機的重置腳位
#define SS 10 // 晶片選擇腳位
MFRC522 rfid(SS, RST); // 建立MFRC522物件

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BTSerial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}
char message[9];
void loop() {
  // put your main code here, to run repeatedly:
    while((rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())){   
      //MotorWriting(150,150);
    
    byte *id = rfid.uid.uidByte; // 取得卡片的UID
    byte idSize = rfid.uid.size; // 取得UID的長度
    // 根據卡片回應的SAK值
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    BTSerial.write("get");
    BTSerial.write('\n');
    for (byte i = 0; i < idSize; i++) { // 逐一顯示UID碼
      //message[i]=id[i]; //ㄆㄐ我有寫錯嗎
      //meow = id[i] if id[i]
      // BTSerial.print(setw(2), setfill('0'), id[i], HEX); // 以16進位顯示UID值   
      BTSerial.print(id[i],"%04X");
      Serial.print(id[i],HEX);
    }
    BTSerial.print('\n');
    
    rfid.PICC_HaltA(); // 讓卡片進入停止模式
    }
}
