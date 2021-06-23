#include <TimerEvent.h>
#include "Temperature_Humidity_Online.h"

// Replace with your SSID and password details
const char * ssid = "CLARKMU";
const char * pass = "18622198941"; 

TimerEvent weatherUpdater;
Clarkwise_temp_online cwtemp;

void update_weather_warpper();

void setup(){
	Serial.begin(115200);

	delay(1000); //wait until the clarkwise touch screen start up.

	//Setup printing:
	WiFi.begin(ssid,pass);
	Serial.println("@SET 100,WIFI Connecting");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
	}
	Serial.println("@SET 100,WIFI Connected."); //Update on the status bar.

	delay(500);
	cwtemp.where_am_i();

  delay(500);
	cwtemp.update_weather();

  weatherUpdater.set(1800000, update_weather_warpper); //Update weather information every 30 mins
}

void loop(){
  weatherUpdater.update();
}

void update_weather_warpper(){ //Can't just pass a pointer to a member function. Warp it up.
	cwtemp.update_weather();
}
