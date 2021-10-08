int ENB = 6;//5
int ENA = 5;//6
int IN1 = 4;//8
int IN2 = 3;//7
int IN3 = 8;//4
int IN4 = 7;//3

#define Rx 2
#define Tx 9
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(Rx, Tx);
char cmmd = 't';

#define L2 A0
#define L1 A1
#define M A2
#define R1 A3
#define R2 A4

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
}
bool block = false;
void loop() {
  // put your main code here, to run repeatedly:
//  if (BTSerial.available()){
//    cmmd = char(BTSerial.read());}
//  if(cmmd == 'f'){MotorWriting(80,80);}
//  else if(cmmd == 'b'){MotorWriting(-200,-200);}
//  else if(cmmd == 'r'){MotorWriting(100,200);}  
//  else if(cmmd == 'l'){MotorWriting(200,100);}
//  else if(cmmd == 't'){
    Tracking();
//    corner();}
}

void MotorWriting(double vR, double vL){
  if(vL>=0){
     digitalWrite(IN1,LOW);  
     digitalWrite(IN2,HIGH);
  } else if(vL<0){
     digitalWrite(IN1,HIGH);
     digitalWrite(IN2,LOW);
    vL=-vL;
  }
  if(vR>=0){
     digitalWrite(IN3,LOW);
     digitalWrite(IN4,HIGH);
  } else if(vR<0){
     digitalWrite(IN3,HIGH);  
     digitalWrite(IN4,LOW);
    vR=-vR;
  }
  analogWrite(ENA,vL);
  analogWrite(ENB,vR);
}

void MotorCheck()
{
  MotorWriting(200,200);
  delay(2000);
  MotorWriting(-200,-200);
  delay(2000);
  MotorWriting(100,200);
  delay(2000);
  MotorWriting(200,100);
  delay(2000);
}

int pre_time=millis();
int cur_time=millis();
double pre_error=0;
int Kp=75; //比例增益係數
int Ki=25; //微分增益係數

void Tracking(){
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);
  if((l1 + l2 + m + r1 + r2) != 0){
  double error = (1*l1+0.5*l2-0.75*(l1==l2&&l1==1)-0.25*(m==l2&&m==1))-(1*r1+0.5*r2-0.75*(r1==r2&&r1==1)-0.25*(m==r2&&m==1));
  double LastError = (error - pre_error);
  pre_error = error;
  double vR = 125+Kp*error+Ki*LastError;
  double vL = 125-Kp*error-Ki*LastError;
  MotorWriting(int(vR),int(vL));
  }
  else MotorWriting(-150,-150);
}

void corner(){
  int l2 = digitalRead(L2);
  int l1 = digitalRead(L1);
  int m = digitalRead(M);
  int r1 = digitalRead(R1);
  int r2 = digitalRead(R2);
  if(l2*l1*m*r1*r2==1){
    block=true;
  }
  if(block && l2*r2==0){
    MotorWriting(0,0);
    if(m==1){}//車子通過轉彎區
    else{}//車子到達死巷
    delay(1000);
    block=false;
  }
}
