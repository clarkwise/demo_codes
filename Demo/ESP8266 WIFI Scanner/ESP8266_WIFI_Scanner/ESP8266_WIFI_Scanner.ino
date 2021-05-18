/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "ESP8266WiFi.h"

void setup() {
  Serial.begin(9600);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  serialEvent();
}

void serialEvent() {
   int received_var_int;
   float received_var_float;
   String received_var_string;
   unsigned short body_id = ATFMessageService(&received_var_int, &received_var_float, &received_var_string);
   //Handle the returned data here or you can store the data and handle it later in the main loop.
  if(body_id==101 && received_var_int==1){
    scan();
  }
}

void scan(){
  Serial.println("@CTRL 101,0"); //Disable the Scan button
  
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();

  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print("@CTRL 100,0x20," + String(i));
      Serial.print(", SSID (HIDE)");
      //Serial.print(WiFi.SSID(i)); SSID is hide in the demo. 
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  
  Serial.println("@CTRL 101,1"); //Enable the Scan button
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
       yield();
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
