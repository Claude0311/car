int ENB = 6;
int ENA = 5;
int IN1 = 8;
int IN2 = 7;
int IN3 = 4;
int IN4 = 3;


void setup() {
  // put your setup code here, to run once:
  pinMode(ENB, OUTPUT);
  pinMode(ENA, OUTPUT);
  
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(ENB,255);
  analogWrite(ENA,255);
  digitalWrite(IN1,HIGH);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(2000);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(2000);
  digitalWrite(IN1,HIGH);  
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(2000);
  digitalWrite(IN1,LOW);  
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(2000);
/*  for(int i=5;i>=0;i--)
  {
      analogWrite(ENB,50*i);
      analogWrite(ENA,50*i);
      delay(500);
  }*/
}
