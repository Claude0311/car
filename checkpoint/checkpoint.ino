//迷宮自走
int ENB = 6;
int ENA = 5;
int IN1 = 8;
int IN2 = 7;
int IN3 = 4;
int IN4 = 3;

#define Rx 2
#define Tx 9
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(Rx, Tx);

String cmmd="fffrbfrlrbfs";//"llrrbrllbflbs";//"lflbrfrb";//
boolean haveCard=true;

char cmmmd='a';
int step0 = 0;
#define L2 A1
#define L1 A2
#define M A3
#define R1 A4
#define R2 A5

#include <SPI.h>
#include <MFRC522.h> // 引用程式庫
#define RST A0 // 讀卡機的重置腳位
#define SS 10 // 晶片選擇腳位
MFRC522 rfid(SS, RST); // 建立MFRC522物件
double pre_error=0;
void setup() {
  // put your setup code here, to run once:
  pinMode(ENB, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(L2 , INPUT);
  pinMode(L1 , INPUT);
  pinMode(M , INPUT);
  pinMode(R1 , INPUT);
  pinMode(R2 , INPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);
  SPI.begin();
  rfid.PCD_Init(); // 初始化MFRC522讀卡機模組
}

bool block = false;
String fake = "";
void loop() {
  // put your main code here, to run repeatedly:
  //String ReceiveData="";
  char val;
  while(BTSerial.available())
  {
    delay(10);
    val = BTSerial.read();
    //ReceiveData += val;
    fake += val;  
  }
  //Serial.print(cmmd);
//  cmmd = fake + "s";
  //BTSerial.print(cmmd);
  //BTSerial.print('\n');
  if(cmmd!="s"){
      //cmmmd=ReceiveData[0];
      cmmmd = cmmd[0];
  }
  if (cmmmd!='0'){
    if(block){
      Corner();
    }
    else{
      Tracking();
    }
 }
  
}

void MotorWriting(double vR, double vL){
  if(vL>=0){
     digitalWrite(IN1,HIGH);  
     digitalWrite(IN2,LOW);
  } else if(vL<0){
     digitalWrite(IN1,LOW);
     digitalWrite(IN2,HIGH);
    vL=-vL;
  }
  if(vR>=0){
     digitalWrite(IN3,HIGH);
     digitalWrite(IN4,LOW);
  } else if(vR<0){
     digitalWrite(IN3,LOW);  
     digitalWrite(IN4,HIGH);
    vR=-vR;
  }
  analogWrite(ENA,vL*0.6);
  analogWrite(ENB,vR*0.6);
}

int pre_time=millis();
int cur_time=millis();

int Kp=160; //比例增益係數132
int Kd=61; //微分增益係數44
void Tracking(){
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);
  if(l2*l1*m*r1*r2==1){
    if(cmmd[step0]=='s') MotorWriting(0,0);
    else block=true;
   pre_error=0;
  }
  else if((l1 + l2 + m + r1 + r2) != 0){
  double error = (1*l1+0.5*l2-0.75*(l1==l2&&l1==1)-0.25*(m==l2&&m==1))-(1*r1+0.5*r2-0.75*(r1==r2&&r1==1)-0.25*(m==r2&&m==1));//越左越大
  double LastError = (error - pre_error);
  pre_error = error;
  double vR = min(255,255+Kp*error+Kd*LastError);
  double vL = min(255,255-Kp*error-Kd*LastError);//220
  MotorWriting(int(vR),int(vL));
  }
  else{
    //MotorWriting(-255,-255);
    pre_error=0;
    int allWhite=1;
    while((l1 + l2 + m + r1 + r2)==0){
      allWhite += 1;

      if(allWhite%2==1) MotorWriting(-210,210);//-250右轉
      else MotorWriting(255,-255);//左轉
      delay(allWhite*100);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
    }
  }
}

int width_car=10;
int omega=2500;
char message[9];
void Corner(){
  if(cmmd[step0]=='r'){
    delay(100);
    MotorWriting(250*(7-width_car/2)/(7+width_car/2),250);
    delay(0.6*omega);
    //BTSerial.print("corner");  //以16進位顯示UID值
    //BTSerial.print('\n');
  }
  else if(cmmd[step0]=='l'){
    delay(100);
    MotorWriting(250,250*(7-width_car/2)/(7+width_car/2));
    delay(0.6*omega);
    //BTSerial.print("corner");  //以16進位顯示UID值
    //BTSerial.print('\n');
  }
  else if(cmmd[step0]=='f'){
    int l2 = digitalRead(L2);
    int l1 = digitalRead(L1);
    int m = digitalRead(M);
    int r1 = digitalRead(R1);
    int r2 = digitalRead(R2);
    while((l1*m*r1)==1){//還在block裡
      MotorWriting(250,250);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
    } 
    //BTSerial.print("corner");  //以16進位顯示UID值
    //BTSerial.print('\n');
  }
  else if(cmmd[step0]=='b'){
    if(haveCard){
      while(!(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())){   
        MotorWriting(150,150);
      }
      char out[2];
      byte *id = rfid.uid.uidByte; // 取得卡片的UID
      byte idSize = rfid.uid.size; // 取得UID的長度
      // 根據卡片回應的SAK值
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      //BTSerial.write("get");
      //BTSerial.write('\n');
      for (byte i = 0; i < idSize; i++) { // 逐一顯示UID碼
        //String temp_str = String(sprintf(out,"%02X",id[i]));
        //Serial.print(temp_str);
        BTSerial.print(id[i],HEX);  //以16進位顯示UID值
        BTSerial.print('\n');
      }
      
      rfid.PICC_HaltA(); // 讓卡片進入停止模式
    }
    MotorWriting(250,-250);
    delay(0.42*omega);
  }
/*
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);
  int firstTry=1;
  while((l2+l1+m+r1+r2)==0){//全白
    firstTry += 1;
    if(cmmd[step0]=='r'){//優先往左調整
      MotorWriting(250,0);
    }
    else if(cmmd[step0]=='l'){//優先往右
      MotorWriting(0,250);
    }
    else{
     if(firstTry%2 == 1){
      MotorWriting(0,250);
     }
     else{
      MotorWriting(250,0);
     }
     delay(firstTry*100);
    }
    l2 = digitalRead(L2);
    l1 = digitalRead(L1);
    m = digitalRead(M);
    r1 = digitalRead(R1);
    r2 = digitalRead(R2);
  }*/
  block=false;
  step0=step0+1;
  /*if(step0>sizeof(cmmd)+1){
    step0=0;
  }*/
}
