int ENB = 6;
int ENA = 5;
int IN1 = 8;
int IN2 = 7;
int IN3 = 4;
int IN4 = 3;

#define Rx 2
#define Tx 9
#include <SoftwareSerial.h>
#include <math.h>
SoftwareSerial BTSerial(Rx, Tx);
char cmmd = 't';
String cmds="rlrs";
bool haveCard=true;

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
  while(!Serial);
  Serial.println("Hello");
  BTSerial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}
bool block = false;
void loop() {
  // put your main code here, to run repeatedly:
  while(BTSerial.available()){
    cmds = cmds.substring(0,cmds.length()-1) + char(BTSerial.read()) + 's';
  }
  Tracking();
  corner();
}

void MotorWriting(double vL, double vR){
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
  analogWrite(ENA,vL);
  analogWrite(ENB,vR);
}

double pre_error=0;
int velocity = 125; //速度
int Kp=75; //比例增益係數
int Ki=50; //微分增益係數

void Tracking(){
  //Serial.begin(9600);
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);

  double _velocity = 120;
  double _Error = 0;
  double _LastError = 0;
  double _SumError = 0;
  double _DeltaError = 0;
  double error = 0;

  double Kp = 10;
  double Ki = 4;
  double Kd = 0.5;
  if(l2*l1*m*r1*r2==1){
    return;
  }
  if((l1 + l2 + m + r1 + r2) != 0){  
    double error_mapping_sum = 2*l2+l1+0*m-1*r1-2*r2;
    double error_mapping_count = l2+l1+m+r1+r2;
    //double error_mapping_count = l2+1.5*l1+2*m+1.5*r1+r2;
    _Error = error_mapping_sum/error_mapping_count;
    double _coeff = 1.4;
    _Error = _Error>=0 ? pow(abs(_Error),_coeff) : -pow(abs(_Error),_coeff);
    _DeltaError = _Error - _LastError;
    _SumError = error + _LastError;

    error = Kp*_Error + Ki*_SumError + Kd*_DeltaError;
    //velocity = _velocity * (30.0-_Error) / 30.0;
    velocity = _velocity;
    
    _LastError = _Error;
    double vR = velocity + error;
    double vL = velocity - error;
    MotorWriting(int(vL),int(vR));
    Serial.print(l2);
    Serial.print(l1);
    Serial.print(m);
    Serial.print(r1);
    Serial.print(r2);
    Serial.print(" ");
    Serial.print(_Error);
    Serial.print(" ");
    Serial.print(_DeltaError);
    Serial.print(" ");
    Serial.print(int(vL));
    Serial.print(" ");
    Serial.print(int(vR));
    Serial.println();

  }else{

    _Error = 0;
    _LastError = 0;
    _SumError = 0;
    _DeltaError = 0;
    error = 0;
    MotorWriting(-90,-90);
  }

  /*
  if((l1 + l2 + m + r1 + r2) != 0){
  //double error = (1*l1 + 0.5*l2 - 0.75*(l1==l2&&l1==1) - 0.25*(m==l2&&m==1)) - (1*r1 + 0.5*r2 - 0.75*(r1==r2&&r1==1) - 0.25*(m==r2&&m==1));
  double error = 0.25*(l2-r2)+0.25*(l1-r1);//-0.25*(l2-m)+0.25*(r2-m);
  double LastError = 0;
  //double LastError = (error - pre_error);
  pre_error = error;
  double vR = velocity + Kp*error + Ki*LastError;
  double vL = velocity - Kp*error - Ki*LastError;
  MotorWriting(int(vR),int(vL));
  //MotorWriting(0,0);
  }
  else {
    MotorWriting(-0.8*velocity,-0.8*velocity);
    //MotorWriting(0,0);
  }*/
}


int step0 = 0;
int width_car=10;
int omega=2000;
void corner(){
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);
//  if(cmds[step0]=='b'){
//    if(haveCard){
//        if((rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())){  
//          char out[2];
//          byte *id = rfid.uid.uidByte; // 取得卡片的UID
//          byte idSize = rfid.uid.size; // 取得UID的長度
//          // 根據卡片回應的SAK值
//          MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
//          //BTSerial.write("get");
//          //BTSerial.write('\n');
//          for (byte i = 0; i < idSize; i++) { // 逐一顯示UID碼
//            //String temp_str = String(sprintf(out,"%02X",id[i]));
//            //Serial.print(temp_str);
//            BTSerial.print(id[i],HEX);  //以16進位顯示UID值
//            BTSerial.print('\n');
//          }
//          rfid.PICC_HaltA(); // 讓卡片進入停止模式
//          step0+=1;
//          MotorWriting(150,-150);
//          delay(0.42*omega);
//          l2 = digitalRead(L2);
//          l1 = digitalRead(L1);
//          m = digitalRead(M);
//          r1 = digitalRead(R1);
//          r2 = digitalRead(R2);
//          while((l2+l1+m+r1+r2)!=1){
//            l2 = digitalRead(L2);
//            l1 = digitalRead(L1);
//            m = digitalRead(M);
//            r1 = digitalRead(R1);
//            r2 = digitalRead(R2);
//            delay(10);
//          }
//        }
//      }
//  }
  if(l2*l1*m*r1*r2==1){
    if(cmds[step0]=='s'){
      MotorWriting(0,0);
    }else{
      MotorWriting(0,0);
      delay(500);
      // l2 = digitalRead(L2);
      // l1 = digitalRead(L1);
      // m = digitalRead(M);
      // r1 = digitalRead(R1);
      // r2 = digitalRead(R2);
      // while(l2*l1*m*r1*r2!=1){
      //   MotorWriting(-100,-100);
      // }
      block=true;
    }
  }
  if(block){
    pre_error=0;
    if(cmds[step0]=='r'){
      MotorWriting(100,-50);
      delay(0.6*omega);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
      while((l2+l1+m+r1+r2)!=1){
        l2 = digitalRead(L2);
        l1 = digitalRead(L1);
        m = digitalRead(M);
        r1 = digitalRead(R1);
        r2 = digitalRead(R2);
        delay(10);
      }
      BTSerial.print("turn right");  //以16進位顯示UID值
      BTSerial.print('\n');
    }
    else if(cmds[step0]=='l'){
//      delay(100);
      MotorWriting(0,100);
      delay(0.6*omega);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
      while((l2+l1+m+r1+r2)!=1){
        l2 = digitalRead(L2);
        l1 = digitalRead(L1);
        m = digitalRead(M);
        r1 = digitalRead(R1);
        r2 = digitalRead(R2);
        delay(10);
      }
      BTSerial.print("turn left");  //以16進位顯示UID值
      BTSerial.print('\n');
    }
    else if(cmds[step0]=='f'){
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
      while((l1*m*r1)==1){//還在block裡
        MotorWriting(100,100);
        l2 = digitalRead(L2);
        l1 = digitalRead(L1);
        m = digitalRead(M);
        r1 = digitalRead(R1);
        r2 = digitalRead(R2);
      } 
      BTSerial.print("straight");  //以16進位顯示UID值
      BTSerial.print('\n');
    }
    else if(cmds[step0]=='b'){
      MotorWriting(150,-150);
      delay(0.10*omega);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
      while((l2+l1+m+r1+r2)!=1){
        l2 = digitalRead(L2);
        l1 = digitalRead(L1);
        m = digitalRead(M);
        r1 = digitalRead(R1);
        r2 = digitalRead(R2);
        delay(10);
      }
      BTSerial.print("back");  //以16進位顯示UID值
      BTSerial.print('\n');
    }
    block=false;
    if(cmds[step0]!='s'){
      step0+=1;
    }
  }
}
