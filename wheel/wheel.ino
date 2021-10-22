int ENB = 6;
int ENA = 5;
int IN1 = 8;
int IN2 = 7;
int IN3 = 4;
int IN4 = 3;

void MotorWriting(double vL, double vR){ // set wheel velocity
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

void setup() {
  // put your setup code here, to run once:
  pinMode(ENB, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
}

double _rvel = 0;
double _lvel = 0;
double rV = 110;
double lV = 110;
double V = 110;
void SmoothVel(double *vel, double *target_vel){
  *vel = *vel + (*target_vel - *vel)*0.9;
}
int i = 0;
void loop() {
  // put your main code here, to run repeatedly:
    if(i%40==0){

      rV = 110;
      lV = 110;
    }else if(i%40 == 10){
      rV = -110;
      lV = -110;
    }else if(i%40 == 20){
      rV = 110;
      lV = 0;
    }else if(i%40 == 30){
      rV = 0;
      lV = 110;
    }      
    SmoothVel(&_rvel, &rV);
    SmoothVel(&_lvel, &lV);
    MotorWriting(_lvel, _rvel);
    delay(300);
    i++;
  /*
  analogWrite(ENB,smoothVel(&_vel, V));
  analogWrite(ENA,smoothVel(&_vel, V));
  digitalWrite(IN1,HIGH);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3000);
  analogWrite(ENB,smoothVel(&_vel, V));
  analogWrite(ENA,smoothVel(&_vel, V));
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3000);
  digitalWrite(IN1,HIGH);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3000);
  digitalWrite(IN1,LOW);  
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3000);
  digitalWrite(IN1,HIGH);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  delay(3000);
  digitalWrite(IN1,LOW);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3000);
  digitalWrite(IN1,LOW);  
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  delay(3000);
  digitalWrite(IN1,LOW);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3000);
  */
/*  for(int i=5;i>=0;i--)
  {
      analogWrite(ENB,50*i);
      analogWrite(ENA,50*i);
      delay(500);
  }*/
}
