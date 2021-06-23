#ifndef TEMPERATURE_HUMIDITY_ONLINE_H_
#define TEMPERATURE_HUMIDITY_ONLINE_H_

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//struct weather icon
struct icon {
  uint8_t index;
  String iname;
};
    
//Struct Control ID
struct cIDs{
  uint8_t id_weekday; //Control ID of a string input control to disply weekday
  uint8_t id_temp_h; //Control ID of a number input control to disply highest temperature
  uint8_t id_temp_l; //Control ID of a number input control to disply lowest temperature
};

class Clarkwise_temp_online{

private:
  uint8_t find_icon_id(String icon_name);
  String http_get(String url);

  HTTPClient http;
  WiFiClient client;
  
public:
  //Month Name
  const char *monthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  //Week Day Name
  const char *weekDay[7] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

  //Control IDs for each day after today. 
  cIDs cIDs_day[7]={{103,104,105},{108,114,115},{109,116,117},{110,121,118},{111,119,120},{112,125,122},{113,123,124}};

  //Base position of weather ICON
  uint16_t ICO_BASE_LEFT = 280;
  uint16_t ICO_OFFSET = 109; //icon left location of each day.
  uint16_t ICO_TOP = 228;

  //The control to update the current status. Like a status bar
  uint16_t ctrlID_status_display = 100;

  //The control to display current weather condition.
  uint16_t ctrlID_current_temp = 102;
  uint16_t ctrlID_current_month_date = 101;
  uint16_t ctrlID_current_humidity = 106;
  uint16_t ctrlID_current_uvi = 107;

  //OpenWeatherMap API Key
  String OWM_API = "6de2c62f3771c3b7d93e2f4a0220b606"; //Your OpenWeatherMap API key here

  //position
  float lat;
  float lon;

  //weather_icon list
  icon weather_icon[18] = {{2, "01d"}, {3, "01n"}, {4, "02d"}, {5, "02n"}, {6, "03d"}, {7, "03n"}, {8, "04d"}, {9, "04n"}, {10, "09d"}, {11, "10d"}, {12, "11d"}, {13, "13d"}, {14, "50d"}, {11, "10n"}, {10, "09n"}, {12, "11n"}, {13, "13n"}, {14, "50n"}};

  void update_weather();
  void where_am_i();
};
#endif
