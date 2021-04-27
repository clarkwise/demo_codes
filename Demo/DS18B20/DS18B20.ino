#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int refreshRate = 1;
float temperature = 0.0;
int maxGrid = 300;
int topTemperature = 100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  sensors.begin();

  delay(2000);

  Serial.println("@SET 100," + String(refreshRate));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  Serial.println("@SET 101," + String(temperature));
  Serial.println("@SET 102," + String(temperature));

  delay(1000/refreshRate);
}

void serialEvent() {
   int received_var_int;
   float received_var_float;
   String received_var_string;
   unsigned short body_id = ATFMessageService(&received_var_int, &received_var_float, &received_var_string);
   //Handle the returned data here or you can store the data and handle it later in the main loop.
  if(body_id==100){
    refreshRate = received_var_int;
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
