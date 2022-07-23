#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "secrets.h" // WIFI credentials, API key, location info

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println();
  Serial.print("Connecting to " WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected, trying HTTP request");

  WiFiClient client;
  HTTPClient http;
  http.useHTTP10(true);
  http.begin(client, "http://api.openweathermap.org/data/2.5/weather?units=imperial&zip=" ZIP_CODE "&appid=" API_KEY);
  http.GET();

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, http.getStream());

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  /** Unused JSON is commented out **/
  // float coord_lon = doc["coord"]["lon"];
  // float coord_lat = doc["coord"]["lat"];

  // JsonObject weather_0 = doc["weather"][0];
  // int weather_0_id = weather_0["id"];                           // 801
  // const char *weather_0_main = weather_0["main"];               // "Clouds"
  // const char *weather_0_description = weather_0["description"]; // "few clouds"
  // const char *weather_0_icon = weather_0["icon"];               // "02d"

  // const char *base = doc["base"]; // "stations"

  JsonObject main = doc["main"];
  // float main_temp = main["temp"];             // 94.69
  float main_feels_like = main["feels_like"]; // 96.46
  // float main_temp_min = main["temp_min"];     // 88.75
  // float main_temp_max = main["temp_max"];     // 100.02
  // int main_pressure = main["pressure"];       // 1015
  // int main_humidity = main["humidity"];       // 36

  // int visibility = doc["visibility"]; // 10000

  // float wind_speed = doc["wind"]["speed"]; // 12.66
  // int wind_deg = doc["wind"]["deg"];       // 190

  // int clouds_all = doc["clouds"]["all"]; // 20

  // long dt = doc["dt"]; // 1658605567

  // JsonObject sys = doc["sys"];
  // int sys_type = sys["type"];               // number
  // long sys_id = sys["id"];                  // number
  // const char *sys_country = sys["country"]; // "US"
  // long sys_sunrise = sys["sunrise"];        // 1658569429
  // long sys_sunset = sys["sunset"];          // 1658621999

  // int timezone = doc["timezone"]; // number
  // int id = doc["id"];             // 0
  // const char *name = doc["name"];
  // int cod = doc["cod"]; // 200 (http code)

  http.end();
  Serial.print("feels like: ");
  Serial.println(main_feels_like);
}

void loop()
{
}