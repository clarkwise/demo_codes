#include "Temperature_Humidity_Online.h"

//return the icon id based on the icon name returned from OpenWeatherAPI
uint8_t Clarkwise_temp_online::find_icon_id(String icon_name){
	uint8_t array_size = sizeof(this->weather_icon)/sizeof(*this->weather_icon);
	for(uint8_t i=0;i<array_size;i++){
		if(this->weather_icon[i].iname==icon_name){
			return this->weather_icon[i].index;
		}
	}
}

String Clarkwise_temp_online::http_get(String url){
  String input;
  this->client.stop();
  this->http.begin(this->client, url.c_str()); 
  int httpCode = http.GET();
  if (httpCode == 200) { //Check the returning code
  	input = http.getString();
  } else {
  	//Serial.printf("[HTTP] GET... code: %d\n", httpCode); //For debugging
  }
  this->http.end(); //Close connection
  return input;
}

void Clarkwise_temp_online::update_weather(){
	Serial.printf("@SET %d,Updating weather data.\n", this->ctrlID_status_display); //Update on the status bar.
	String url = "http://api.openweathermap.org/data/2.5/onecall?lat=" + String(this->lat) + "&lon=" + String(this->lon) + "&exclude=minutely,hourly,alerts&units=metric&lang=en&appid=" + this->OWM_API;
	String input = this->http_get(url);

	DynamicJsonDocument doc(6200);
	DeserializationError error = deserializeJson(doc, input);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}
	JsonObject current = doc["current"];
	long current_dt = current["dt"]; 
	float current_temp = current["temp"];
	int current_humidity = current["humidity"];
	float current_uvi = current["uvi"];

	JsonObject current_weather_0 = current["weather"][0];
	const char* current_weather_0_icon = current_weather_0["icon"];

	//Update controls
	Serial.printf("@SET %d,%.1fC\n", this->ctrlID_current_temp, current_temp);
	
	time_t t = current_dt;
	tm *c_ptm = gmtime(&t);

	Serial.printf("@SET %d,%s %d\n", this->ctrlID_current_month_date, this->monthName[c_ptm->tm_mon], c_ptm->tm_mday);
	Serial.printf("@SET %d,%d%%\n", this->ctrlID_current_humidity, current_humidity);
	Serial.printf("@SET %d,%.2f\n", this->ctrlID_current_uvi, current_uvi);
	Serial.printf("@BMP %d,%d,%d\n", this->ICO_BASE_LEFT, ICO_TOP, find_icon_id(String(current_weather_0_icon)));

	uint8_t dayNum = 0;

	for (JsonObject elem : doc["daily"].as<JsonArray>()) {

		long dt = elem["dt"];

		JsonObject temp = elem["temp"];
		float temp_min = temp["min"];
		float temp_max = temp["max"];

		JsonObject weather_0 = elem["weather"][0];

		const char* weather_0_icon = weather_0["icon"];

		//Process the data
		if(dayNum<7){
			t = dt;
			tm *ptm = gmtime (&t);
			Serial.printf("@SET %d,%.1f\n", this->cIDs_day[dayNum].id_temp_h, temp_max);
			Serial.printf("@SET %d,%.1f\n", this->cIDs_day[dayNum].id_temp_l, temp_min);
			Serial.printf("@SET %d,%s\n", this->cIDs_day[dayNum].id_weekday, this->weekDay[ptm->tm_wday]);
			if(dayNum>0){
				Serial.printf("@BMP %d,%d,%d\n", this->ICO_BASE_LEFT + dayNum * this->ICO_OFFSET, this->ICO_TOP, find_icon_id(String(weather_0_icon)));
			}
		}
		dayNum++;
	}
	Serial.printf("@SET %d,Weather updated.", this->ctrlID_status_display);
}

void Clarkwise_temp_online::where_am_i(){

	Serial.printf("@SET %d,Obtaining Latitude and Longitude.\n", this->ctrlID_status_display); //Update on the status bar.
	String input = this->http_get("http://ip-api.com/json/?fields=lat,lon");

	StaticJsonDocument<96> doc;
	DeserializationError error = deserializeJson(doc, input);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	this->lat = doc["lat"];
	this->lon = doc["lon"];

	Serial.printf("@SET %d,Lat: %.2f, Lon: %.2f\n", this->ctrlID_status_display, this->lat, this->lon); //Update on the status bar.
}
