#include <Bounce2.h>

#define data 6
#define clock  7
#define enable 8
#define dig2 2
#define dig3 3
#define dig4 4
#define an4 18
#define an5 19
#define v_in 14
#define l_in 15
#define sig_out 9

byte disp_mode = 0;
float v=0.0;
float l=0.0;
int lo=0;

Bounce sw = Bounce();

//--------------------------------------------------
float mapf(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//--------------------------------------------------
float readVolt(){
  for(int i=0; i<100; i++){
    v+=analogRead(v_in);
  }
  v=v/100;
  return mapf(v,0.0,1023.0,0.0,15.5);
}

float readLambda(){
  for(int i=0; i<100; i++){
    l+=analogRead(l_in);
  }
  l=l/100;
  return mapf(l,0.0,254.0,19.7,9.0);
}

int LtoSigOut(){
  for(int i=0; i<100; i++){
    lo+=analogRead(l_in);
  }
  lo=lo/100;
  return map(lo,0,254,8,154);
}

int LedTest(){
  analogWrite(sig_out,0);
  delay(5);
  for(int i=8; i<153; i++){
    analogWrite(sig_out,i);
    delay(5);
  }
  for(int i=153; i>8; i--){
    analogWrite(sig_out,i);
    delay(5);
  }
}

//--------------------------------------------------
void display(float x){
  int num[2];
  int i=0;
  byte a=0;
  byte b=0;
  byte c=0; 
  byte conf=0;

  i = int(x*10);

  if(i<100) conf=B00001001;
  else if(i<10) conf=B00001101;
  else conf=B00000001;

  num[0] = int ( i / 100 );
  i = i % 100; 
  num[1] = int ( i / 10 );
  i = i % 10; 
  num[2] = i % 10; 

  c = num[1];
  c = c << 4;
  c = c | num[2];

  b = B00000000 | num[0]; 
  
  a=B10100000; 

  digitalWrite(enable, LOW);
  shiftOut(data, clock, MSBFIRST, conf);
  digitalWrite(enable, HIGH);

  delay(10);

  digitalWrite(enable, LOW);
  shiftOut(data, clock, MSBFIRST, a);
  shiftOut(data, clock, MSBFIRST, b);
  shiftOut(data, clock, MSBFIRST, c); 
  digitalWrite(enable, HIGH);

  delay(10);
}
//--------------------------------------------------
void displayOn(){
 digitalWrite(enable, LOW);
 shiftOut(data, clock, MSBFIRST, B00000001);
 digitalWrite(enable, HIGH);
 delay(10);
}
void displayOff(){
 digitalWrite(enable, LOW);
 shiftOut(data, clock, MSBFIRST, B00000000);
 digitalWrite(enable, HIGH);
 delay(10);
}
//--------------------------------------------------
void displayTest(){
 byte a=0; 
 byte b=0; 
 byte c=0; 

 for(int i = 0; i<10; i++){
  c = i;
  c = c << 4;
  c = c | i;

  b = B00000000 | i;

  if(i==0) a=B11110000;
  else a=B10000000;

  digitalWrite(enable, LOW);
  shiftOut(data, clock, MSBFIRST, a);
  shiftOut(data, clock, MSBFIRST, b);
  shiftOut(data, clock, MSBFIRST, c); 
  digitalWrite(enable, HIGH);

  delay(250);
 }
 delay(100); 
}
//--------------------------------------------------
void setup() {
  analogReference(DEFAULT);

  pinMode(data, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(dig2, INPUT_PULLUP);

  sw.attach(dig2); 
  sw.interval(50);
  delay(1000);
  displayOn();
  displayTest();
  delay(300);
  LedTest();
}

void loop() {

  analogWrite(sig_out,LtoSigOut());

  sw.update();
  if (sw.fell()) {
    disp_mode+=1; 
  }
  if(disp_mode>=3) disp_mode=0;

  switch(disp_mode){
    case 0: display(readLambda());
            break;
    case 1: display(readVolt());
            break;
    case 2: displayOff();
            break;
  }  
  //displayOn();
}
