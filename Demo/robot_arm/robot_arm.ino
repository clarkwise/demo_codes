#include <Servo.h>

bool f_pick = false;

#define servo_qty 6
Servo robot_arm[servo_qty];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  delay(1200);

  Serial.println("@SET 500,300");

  for(uint8_t i=100;i<=111;i++){
    Serial.println("@SET " + String(i) + ",90");
    delay(20);
  }
  
  robot_arm[0].attach( 3, 500, 2500);
  robot_arm[1].attach( 6, 500, 2500);
  robot_arm[2].attach( 5, 500, 2500);
  robot_arm[3].attach( 9, 500, 2500);
  robot_arm[4].attach( 10, 500, 2500);
  robot_arm[5].attach( 11, 500, 2500);

}

void loop() {
  // put your main code here, to run repeatedly:
}

void serialEvent() {
   int received_var_int;
   float received_var_float;
   String received_var_string;
   unsigned short body_id = ATFMessageService(&received_var_int, &received_var_float, &received_var_string);
   //Handle the returned data here or you can store the data and handle it later in the main loop.

   switch (body_id){
      case 100:
        robot_arm[0].write(received_var_int);
        break;
      case 101:
        robot_arm[1].write(received_var_int);
        break;
      case 102:
        robot_arm[2].write(received_var_int);
        break;
      case 103:
        robot_arm[3].write(received_var_int);
        break;
      case 104:
        robot_arm[4].write(received_var_int);
        break;
      case 105:
        robot_arm[5].write(received_var_int);
        break;
      case 200:
        f_pick = true;
        break;

      case 250:
        for(int i=1;i<6;i++){
          robot_arm[i].write(90);
        }
        break;

      case 500:
        Serial.println("@BKL " + String(received_var_int));
        break;        
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
