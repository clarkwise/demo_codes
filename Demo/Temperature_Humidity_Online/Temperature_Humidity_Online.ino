#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <TimerEvent.h>


// Replace with your SSID and password details
const char * ssid = "your-ssid";
const char * pass = "your-password"; 

HTTPClient http;
WiFiClient client;

TimerEvent weatherUpdater;

void where_am_i();
void update_weather();
uint8_t find_icon_id(String icon_name);
String http_get(String url);

float my_lat;
float my_lon;

//Base position of weather ICON
#define ICO_BASE_LEFT 280
#define ICO_OFFSET 109
#define ICO_TOP 228

//weather_icon list
#define WEA_ICO_0 {2, "01d"}
#define WEA_ICO_1 {3, "01n"}
#define WEA_ICO_2 {4, "02d"}
#define WEA_ICO_3 {5, "02n"}
#define WEA_ICO_4 {6, "03d"}
#define WEA_ICO_5 {7, "03n"}
#define WEA_ICO_6 {8, "04d"}
#define WEA_ICO_7 {9, "04n"}
#define WEA_ICO_8 {10, "09d"}
#define WEA_ICO_9 {11, "10d"}
#define WEA_ICO_10 {12, "11d"}
#define WEA_ICO_11 {13, "13d"}
#define WEA_ICO_12 {14, "50d"}
#define WEA_ICO_13 {11, "10n"}
#define WEA_ICO_14 {10, "09n"}
#define WEA_ICO_15 {12, "11n"}
#define WEA_ICO_16 {13, "13n"}
#define WEA_ICO_17 {14, "50n"}

typedef struct icon {
  uint8_t index;
  String iname;
};
const icon weather_icon[18] = {WEA_ICO_0, WEA_ICO_1, WEA_ICO_2, WEA_ICO_3, WEA_ICO_4, WEA_ICO_5, WEA_ICO_6, WEA_ICO_7, WEA_ICO_8, WEA_ICO_9, WEA_ICO_10, WEA_ICO_11, WEA_ICO_12, WEA_ICO_13, WEA_ICO_14, WEA_ICO_15, WEA_ICO_16, WEA_ICO_17};

//Week Day Name
const char *weekDay[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

//Month Name
const char *monthName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
//Control IDs
struct cIDs{
  uint8_t id_weekday;
  uint8_t id_temp_h;
  uint8_t id_temp_l;
};
const cIDs cIDs_day[7]={{103,104,105},{108,114,115},{109,116,117},{110,121,118},{111,119,120},{112,125,122},{113,123,124}};

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
	where_am_i();

  delay(500);
	update_weather();

  weatherUpdater.set(1800000, update_weather); //Update weather information every 30 mins
}

void loop(){
  weatherUpdater.update();
}

void where_am_i(){
  
	Serial.println("@SET 100,Obtaining Latitude and Longitude."); //Update on the status bar.
	String input = http_get("http://ip-api.com/json/?fields=lat,lon");

  delay(500);

	StaticJsonDocument<96> doc;
	DeserializationError error = deserializeJson(doc, input);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	my_lat = doc["lat"];
	my_lon = doc["lon"];

	Serial.printf("@SET 100,Lat: %.2f, Lon: %.2f\n", my_lat, my_lon); //Update on the status bar.
}

void update_weather(){

  Serial.println("@SET 100,Updating weather data."); //Update on the status bar.
  
	const String API_key = "openweathermap-api-key-here"; //Your OpenWeatherMap API key here
	String url = "http://api.openweathermap.org/data/2.5/onecall?lat=" + String(my_lat) + "&lon=" + String(my_lon) + "&exclude=minutely,hourly,alerts&units=metric&lang=en&appid=" + API_key;
	String input = http_get(url);

  delay(500);

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
  Serial.printf("@SET 102,%.1fC\n", current_temp);
  
  time_t t = current_dt;
  tm *c_ptm = gmtime(&t);

  Serial.printf("@SET 101,%s %d\n", monthName[c_ptm->tm_mon], c_ptm->tm_mday);
  Serial.printf("@SET 106,%d%%\n", current_humidity);
  Serial.printf("@SET 107,%.2f\n", current_uvi);
  Serial.printf("@BMP %d,%d,%d\n", ICO_BASE_LEFT, ICO_TOP, find_icon_id(String(current_weather_0_icon)));

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
      Serial.printf("@SET %d,%.1f\n", cIDs_day[dayNum].id_temp_h, temp_max);
      Serial.printf("@SET %d,%.1f\n", cIDs_day[dayNum].id_temp_l, temp_min);
      Serial.printf("@SET %d,%s\n",cIDs_day[dayNum].id_weekday, weekDay[ptm->tm_wday]);
      if(dayNum>0){
      	Serial.printf("@BMP %d,%d,%d\n", ICO_BASE_LEFT + dayNum * ICO_OFFSET, ICO_TOP, find_icon_id(String(weather_0_icon)));
      }
    }
    dayNum++;
	}
  Serial.println("@SET 100,Weather updated.");
}

String http_get(String url){
  String input;
  client.stop();
  
	http.begin(client, url.c_str()); 
  
	int httpCode = http.GET();

	if (httpCode == 200) { //Check the returning code
		input = http.getString();
	} else {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
	}
	http.end(); //Close connection
	return input;
}

uint8_t find_icon_id(String icon_name){
	for(uint8_t i=0;i<18;i++){
		if(weather_icon[i].iname==icon_name){
			return weather_icon[i].index;
		}
	}
}
  
