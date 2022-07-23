#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Weather.h>

#include "secrets.h" // WIFI credentials

Weather weather;

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

  weather.update();

  Serial.print("feels like: ");
  Serial.println(weather.getFeelsLike());
}

void loop()
{
}