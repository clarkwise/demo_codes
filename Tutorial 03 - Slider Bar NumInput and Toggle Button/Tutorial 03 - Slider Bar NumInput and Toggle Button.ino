#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int pos=0;
int sweep_mode=0;
int adding = 1;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);
  Serial.begin(9600);
  myservo.write(pos); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  if(sweep_mode==1){
    if(pos>180){
      adding=-1;
    }
    if(pos<0){
      adding=1;
    }
    pos = pos+adding;
    myservo.write(pos); 
  }
}


void serialEvent() {
   int received_var_int;
   float received_var_float;
   String received_var_string;
   unsigned short body_id = ATFMessageService(&received_var_int, &received_var_float, &received_var_string);
   //Handle the returned data here or you can store the data and handle it later in the main loop.
  if(body_id==101 || body_id==100){
    pos = received_var_int;
    myservo.write(pos);
  }
  if(body_id==102){
    if(received_var_int==1){
      sweep_mode=1;
    }else{
      sweep_mode=0;
    }
  }
}
unsigned short ATFMessageService(int *m_ReciverVarInt, float *m_ReciverVarFloat, String *m_ReciverVarString)
 {
   char n_TempChar;
   unsigned short m_ReciverBodyID;
   n_TempChar = Serial.available();
   if(n_TempChar)
   {
     delay(2); // 2ms delay to make sure the data is successfully received.
     n_TempChar = Serial.read();
     while(n_TempChar!='@'&&n_TempChar!='#'&&n_TempChar!='$'&&n_TempChar>=0)
     {
       n_TempChar = Serial.read();
     }
   m_ReciverBodyID = Serial.parseInt();
   if(n_TempChar=='@') {
      *m_ReciverVarInt = Serial.parseInt(); 
   } else if(n_TempChar=='#') {
      *m_ReciverVarFloat = Serial.parseFloat();
   } else if(n_TempChar=='$') {
      Serial.read();
      *m_ReciverVarString = Serial.readStringUntil('\r');
   }
   return m_ReciverBodyID;
 }
}
