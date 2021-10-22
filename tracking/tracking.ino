#include <SoftwareSerial.h>
#include <math.h>

#define Rx 2
#define Tx 9

int ENB = 6;
int ENA = 5;
int IN1 = 8;
int IN2 = 7;
int IN3 = 4;
int IN4 = 3;

enum Sensor{
  BLACK = 1,
  WHITE = 0
};

SoftwareSerial BTSerial(Rx, Tx);
char cmmd = 't';
String cmds="flrffrrbllrrbfrbllrllrrbfbrflfbrbrfffrrrbllfbrrflrbllrrbs";
//String cmds="ffflrlrbfs";
//String cmds="rrs";
bool haveCard=true;

#define L2 A1
#define L1 A2
#define M A3
#define R1 A4
#define R2 A5

// RFID
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


bool is_corner = false; // check if car is going to do next step

int step0 = 0;
int width_car=10;
int omega=2000; // time

// wheel velocity
int _leftV = 100;
int _rightV = 99;


void loop() {
  // put your main code here, to run repeatedly:
  while(BTSerial.available()){ // read command from bt 
    cmds = cmds.substring(0,cmds.length()-1) + char(BTSerial.read()) + 's';
  }
  Tracking(); // tracking on line
  corner(); // decide direction
}

void MotorWriting(double vL, double vR){ // set wheel velocity
  if(vL>0.1){
     digitalWrite(IN1,HIGH);
     digitalWrite(IN2,LOW);
  } else if(vL<0){
     digitalWrite(IN1,LOW);
     digitalWrite(IN2,HIGH);
     vL=-vL;
  }else{
     digitalWrite(IN1,LOW);
     digitalWrite(IN2,LOW);    
  }
  if(vR>0.1){
     digitalWrite(IN3,HIGH);
     digitalWrite(IN4,LOW);
  } else if(vR<0){
     digitalWrite(IN3,LOW);  
     digitalWrite(IN4,HIGH);
     vR=-vR;
  }else{
     digitalWrite(IN3,LOW);  
     digitalWrite(IN4,LOW);
  }
  analogWrite(ENA,vL);
  analogWrite(ENB,vR);
}

int pre_time=millis();
int cur_time=millis();
double pre_error=0;
int Kp=75; //比例增益係數
int Ki=50; //微分增益係數

// digital sensor
int l2 = 0;
int l1 = 0;
int m = 0;
int r1 = 0;
int r2 = 0;

// black/air -> 1, white -> 0
bool allButOneBlack(int s1,int s2,int s3,int s4,int s5){ // 5 black or 4 black
  return s1+s2+s3+s4+s5>=4*BLACK;
}
bool allButOneWhite(int s1,int s2,int s3,int s4,int s5){ // 5 black or 4 black
  return s1+s2+s3+s4+s5<=1*BLACK;
}
bool allBlack(int s1,int s2,int s3,int s4,int s5){ // 5 black 00000 -> 1 00001 -> 0
  return !(s1|s2|s3|s4|s5);
}
bool oneBlack(int s1,int s2,int s3,int s4,int s5){ // at least 1 black 00001 -> 1 00000 -> 0
  return (s1|s2|s3|s4|s5);
}
bool oneWhite(int s1,int s2,int s3,int s4,int s5){ // at least 1 white 01111 -> 1 11111 -> 0
  return (s1+s2+s3+s4+s5)==1;
}
bool allWhite(int s1,int s2,int s3,int s4,int s5){ // 5 white 00000 -> 1 00001 -> 0
  return (s1+s2+s3+s4+s5)==0;
}

void AlignBlock(){ // let car stop in the edge of block
     double l_coeff = 1; 
     double r_coeff = 1;
     
     if(allButOneBlack(l2,l1,m,r1,r2)){ // still in block
        
        do{
          MotorWriting(_leftV,0.95*_rightV);//forward
          delay(100);
          MotorWriting(0,0);
          delay(80);
          l2 = digitalRead(L2);
          l1 = digitalRead(L1);
          m = digitalRead(M);
          r1 = digitalRead(R1);
          r2 = digitalRead(R2);
        }while(allButOneBlack(l2,l1,m,r1,r2)); // still in block
        // now is out of block
        do{
          /*
          if(allBlack(l2,l1,m,BLACK,BLACK) == true & allBlack(BLACK,BLACK,m,r1,r2) == false)l_coeff=0.95;
          else if(allBlack(l2,l1,m,BLACK,BLACK) == false & allBlack(BLACK,BLACK,m,r1,r2) == true)r_coeff=0.95;
          MotorWriting(-l_coeff*_leftV,-r_coeff*_rightV);//forward
          */
          MotorWriting(-_leftV,-0.98*_rightV);
          delay(100);
          
          MotorWriting(0,0);
          
          delay(80);
          l2 = digitalRead(L2);
          l1 = digitalRead(L1);
          m = digitalRead(M);
          r1 = digitalRead(R1);
          r2 = digitalRead(R2);
       }while(allButOneWhite(l2,l1,m,r1,r2));
       // now is in block
     }else{ // out of block
      do{ // out of block
          MotorWriting(-_leftV,-0.98*_rightV);
          delay(100);
          MotorWriting(0,0);
          delay(80);
          l2 = digitalRead(L2);
          l1 = digitalRead(L1);
          m = digitalRead(M);
          r1 = digitalRead(R1);
          r2 = digitalRead(R2);
      }while(allButOneWhite(l2,l1,m,r1,r2));
       
       // in block

      // do{
      //     MotorWriting(_leftV,_rightV);
      //     delay(100);
      //     MotorWriting(0,0);
      //     delay(80);
      //     l2 = digitalRead(L2);
      //     l1 = digitalRead(L1);
      //     m = digitalRead(M);
      //     r1 = digitalRead(R1);
      //     r2 = digitalRead(R2);
      // }while(allButOneBlack(l2,l1,m,r1,r2));
      //out of block
    }
}

void Tracking(){
 //Serial.begin(9600);
 int l2 = digitalRead(L2);
 int l1 = digitalRead(L1);
 int m = digitalRead(M);
 int r1 = digitalRead(R1);
 int r2 = digitalRead(R2);

 double _Error = 0;
 double _LastError = 0;
 double _SumError = 0;
 double _DeltaError = 0;
 double error = 0;

 double Kp = 10;
 double Ki = 4;
 double Kd = 0.2;
 if(allBlack(l1,l2,m,r1,r2)){
   return;
 }
 if(oneBlack(l1,l2,m,r1,r2)){  
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
   
   _LastError = _Error;
   double vR = _rightV + error;
   double vL = _leftV - error;
   MotorWriting(0.9*vL,0.9*vR);
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

 }else if(allWhite(l1,l2,m,r1,r2)){

   _Error = 0;
   _LastError = 0;
   _SumError = 0;
   _DeltaError = 0;
   error = 0;
   MotorWriting(-0.9*_leftV,-0.9*_rightV);
 }

 /*
 // old code
 if((l1 + l2 + m + r1 + r2) != 0){
 double error = (1*l1 + 0.5*l2 - 0.75*(l1==l2&&l1==1) - 0.25*(m==l2&&m==1)) - (1*r1 + 0.5*r2 - 0.75*(r1==r2&&r1==1) - 0.25*(m==r2&&m==1));
 double error = 0.25*(l2-r2)+0.25*(l1-r1);//-0.25*(l2-m)+0.25*(r2-m);
 double LastError = 0;
 double LastError = (error - pre_error);
 pre_error = error;
 double vR = velocity + Kp*error + Ki*LastError;
 double vL = velocity - Kp*error - Ki*LastError;
 MotorWriting(int(vR),int(vL));
 MotorWriting(0,0);
 }
 else {
   MotorWriting(-0.8*velocity,-0.8*velocity);
   MotorWriting(0,0);
 }*/
}



void corner(){

// if found a block, turn to corner mode
// after setup the position on the edge of block, stop or
//turn r/l or go straight or back
// when car done the motion and find the line, finish the corner mode and let tracking do the thing
// read sensor

l2 = digitalRead(L2);
l1 = digitalRead(L1);
m = digitalRead(M);
r1 = digitalRead(R1);
r2 = digitalRead(R2);
 if(cmds[step0]=='b'){
   if(haveCard){
       if((rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())){  
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
          haveCard = false;
          /*
          step0+=1;
          MotorWriting(150,-150);
          delay(0.42*omega);
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
          */
        }
      }
  }else{
    haveCard = true;  
  }

if(allButOneBlack(l2,l1,m,r1,r2)){ // 11111=========== detect block
  if(cmds[step0]=='s'){ // ================= stop ================= 
    MotorWriting(0,0);
    delay(500);
    //AlignBlock();
    is_corner = false;
  }else if(cmds[step0]=='f'){
    MotorWriting(0,0);
    delay(500);
    is_corner = true;
  }else{ // ================= don't stop =================
    MotorWriting(0,0);
    delay(500);
    AlignBlock();

    // now the sensor should aligned to block edge
    delay(200);
    is_corner = true; // should continue
  }
}
if(is_corner){// now the sensor should aligned to block edge
  pre_error=0;
  if(cmds[step0]=='r'){ // ================= right =================
    MotorWriting(0.94*_leftV,0);
    delay(0.34*omega);// 1.2s
//    MotorWriting(0,-_rightV);
//    delay(0.175*omega);// 1.2s
//    
//    do{//all is 0
//  
//      MotorWriting(0.94*_leftV,0);
//      delay(10);
//      l2 = digitalRead(L2);
//      l1 = digitalRead(L1);
//      m = digitalRead(M);
//      r1 = digitalRead(R1);
//      r2 = digitalRead(R2);
//    }while(!oneBlack(l2,l1,m,r1,r2));

    
    int i = 0;
    do{//all is 0
      if(i<0.18*omega){
        MotorWriting(0,-_rightV); 
      }else{
        MotorWriting(0.94*_leftV,0);
      }
      
      i+=10;
      delay(10);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
    }while(!oneBlack(l2,l1,m,r1,r2));
//    BTSerial.print("corner");  //以16進位顯示UID值
//    BTSerial.print('\n');
  }
  else if(cmds[step0]=='l'){ // ================= left =================
//      delay(100);
    MotorWriting(0,1.1*_rightV);
    delay(0.34*omega);
    MotorWriting(-1.2*_leftV,-0);
    delay(0.175*omega);// 1.2s
    do{//all is 0
      MotorWriting(0,1.1*_rightV);
      delay(10);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
    }while(!oneBlack(l2,l1,m,r1,r2));

//    BTSerial.print("corner");  //以16進位顯示UID值
//    BTSerial.print('\n');
  }
  else if(cmds[step0]=='f'){ // ================= forward =================
    
    while(allButOneBlack(l2,l1,m,r1,r2)){//還在block裡
      MotorWriting(_leftV,_rightV);
      delay(10);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
    }
    /*
    int i = 1;
    int num = 50;
    while(allWhite(l2,l1,m,r1,r2)){
      MotorWriting(i*110,-1*i*110);
      delay(num);
      MotorWriting(0,0);
      delay(50);
      num+=50;
      i*=-1;
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);     
    }
    */

//    BTSerial.print("forward");  //以16進位顯示UID值
//    BTSerial.print('\n');
  }
  else if(cmds[step0]=='b'){ // ================= backward =================
      MotorWriting(125,125);
      delay(250);
    // 目前狀態：sensor在末端block邊緣內側
    while(oneBlack(l1,m,r1,WHITE,WHITE)){
      
      MotorWriting(0,0);
      delay(40);
      MotorWriting(-120,100);
      delay(40);
      l2 = digitalRead(L2);
      l1 = digitalRead(L1);
      m = digitalRead(M);
      r1 = digitalRead(R1);
      r2 = digitalRead(R2);
      
    }
    //AlignBlock();
  }
  
  is_corner=false;
  if(cmds[step0]!='s'){
    step0+=1;
  }
 }
}
