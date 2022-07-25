#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "Weather.h"
#include "secrets.h" // API key, location info

bool Weather::update()
{
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.begin(client, "http://api.openweathermap.org/data/2.5/weather?units=imperial&zip=" ZIP_CODE "&appid=" API_KEY);
    http.GET();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, http.getStream());

    if (error)
    {
        /** Uncomment Serial if debugging **/
        // Serial.print(F("deserializeJson() failed: "));
        // Serial.println(error.f_str());
        return false;
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
    temp = main["temp"];             // 94.69
    feels_like = main["feels_like"]; // 96.46
    temp_min = main["temp_min"];     // 88.75
    temp_max = main["temp_max"];     // 100.02
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
    return true;
}