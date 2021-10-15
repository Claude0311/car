#define Rx 2
#define Tx 9

#include <SoftwareSerial.h>
#include <stdlib.h>
SoftwareSerial BTSerial(Rx, Tx);
// Arduino Rx -> Bluetooth Tx // Arduino Tx-> Bluetooth Rx

void setup(){
    Serial.begin(9600);
    BTSerial.begin(9600);
}

void loop()
{
  // Keep reading from HC-05 and send to Arduino Serial Monitor
  String ReceiveData="";
  char val;
  while(BTSerial.available())
  {
    delay(10);
    val = BTSerial.read();
    ReceiveData += val;
  }
  Serial.print(ReceiveData);
 //if (BTSerial.available())
   // Serial.write(BTSerial.read());
  while (Serial.available()){
    //Serial.read();
    delay(1);
    BTSerial.write(Serial.read());
//    BTSerial.write(byte('f'));
//    BTSerial.print(Serial.read());
  }
}
