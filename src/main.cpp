#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h> // for getting time via NTP
#include <Weather.h>

#include "secrets.h" // WIFI credentials, temperatures min/max

void button(int pin, void (*callback)());
time_t getNtpTime();
void servoCalibration();
int temperatureToServo(float temp);
void updateIfNeeded();

Weather weather;
Servo servo;

WiFiUDP udp;
time_t prevTime = 0;

void setup()
{
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(12, LOW);
  digitalWrite(14, LOW);
  servo.attach(13);
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
    ;
#endif

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

#ifdef DEBUG
  Serial.print("connecting");
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef DEBUG
    Serial.print('.');
#endif
    delay(500);
  }

#ifdef DEBUG
  Serial.println(" connected");
#endif

  udp.begin(8888);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  while (timeStatus() == timeNotSet)
    ;
  prevTime = now();

  weather.update();
}

void displayTempMin()
{
  digitalWrite(12, HIGH);
  digitalWrite(14, LOW);
  updateIfNeeded();
  servo.write(temperatureToServo(weather.getTempMin()));
}

void displayTempMax()
{
  digitalWrite(12, LOW);
  digitalWrite(14, HIGH);
  updateIfNeeded();
  servo.write(temperatureToServo(weather.getTempMax()));
}

void loop()
{
  button(2, displayTempMin);
  button(4, displayTempMax);
}

/***** Function definitions *****/

int temperatureToServo(float temp)
{
  // local max/min temperatures for climate are in secrets.h
  return map(temp, TEMP_MAX, TEMP_MIN, 0, 150);
}

// simple, heavyhanded button debounce with callback
void button(int pin, void (*callback)())
{
  if (!digitalRead(pin))
  {
    delay(100);
    {
      if (!digitalRead(pin))
      {
        callback();
        while (!digitalRead(pin))
          delay(100);
      }
    }
  }
}

const int ntpPacketSize = 48;
byte packetBuffer[ntpPacketSize];
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, ntpPacketSize);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); // NTP requests are to port 123
  udp.write(packetBuffer, ntpPacketSize);
  udp.endPacket();
}

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address
  while (udp.parsePacket() > 0)
    ; // discard any previously received packets
  WiFi.hostByName("us.pool.ntp.org", ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = udp.parsePacket();
    if (size >= ntpPacketSize)
    {
      udp.read(packetBuffer, ntpPacketSize); // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL;
    }
  }
  return 0; // return 0 if unable to get the time
}

// for marking the physical scale
void servoCalibration()
{
  for (int i = TEMP_MIN; i <= TEMP_MAX; i += 5)
  {
    Serial.print(i);
    Serial.println("deg F");
    servo.write(temperatureToServo(i));
    delay(2000);
  }
}

void updateIfNeeded()
{
  if (now() - prevTime > SECS_PER_HOUR)
  {
    weather.update();
  }
}
