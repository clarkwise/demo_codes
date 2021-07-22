#include <Wire.h>
#include "Thinary_AHT10.h"
#include "ESP8266_UART_TOUT_INT_clarkwise_handler.h"

AHT10Class AHT10;
unsigned int refreshInterval = 1000;

void setup() {
  // put your setup code here, to run once:
  delay(1200);
  Serial.begin(9600);
  Wire.begin();

  install_uart_tout();
  Serial.println("@SET 102," + String(refreshInterval));
  if(AHT10.begin(eAHT10Address_Low)){
    //Serial.println("Init AHT10 Sucess.");
  } else {
    //Serial.println("Init AHT10 Failure.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("@SET 100," + String(AHT10.GetTemperature()));
  Serial.println("@SET 101," + String(AHT10.GetHumidity()));
  delay(refreshInterval);
}

void clarkwise_returns_handler(){
  if(rMsg.control_id==102){
    refreshInterval = rMsg.varInt;
  }
}
