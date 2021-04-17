void setup() {
   // put your setup code here, to run once:
   Serial.begin(9600);
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
  switch(body_id){
    case 100:
      if(received_var_int==1){
        Serial.println("@STR 0,0,Blue button is pressed.");
      }
      break;
    case 101:
      if(received_var_int==1){
        Serial.println("@STR 0,0,Green button is pressed.");
      }
      break;
    case 102:
      if(received_var_int==1){
        Serial.println("@STR 0,0,Switched to ON");
      } else {
        Serial.println("@STR 0,0,Switched to OFF");
      }
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
