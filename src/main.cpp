// Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

using namespace std;

#define DHTPIN 22 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
const int readChId = 12397;
const int CHANNEL_ID = 3071905;
const String API_KEY = "EFAAM5TKMHRAIXMW";

const char *ssid = "duck";
const char *password = "ducklin404";

void setup()
{
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  dht.begin();
}

void loop()
{
  // Wait a few seconds between measurements.
  delay(15000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    String url = String("http://api.thingspeak.com/update?api_key=") + API_KEY + "&field1=" + String(h) + "&field2=" + String(t) + "&field3=" + String(hic);
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == 200)
    {
      String payload = http.getString();
      Serial.printf("HTTP %d, entry ID: %s\n", httpCode, payload.c_str());
    }
    else
    {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

    url = String("https://api.thingspeak.com/channels/3071905/feeds.json?api_key=") + API_KEY + "&results=1";
    http.begin(url);
    httpCode = http.GET();
    if (httpCode == 200)
    {
      String payload2 = http.getString();
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload2);
      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      JsonArray feeds = doc["feeds"];
      for (JsonObject feed : feeds)
      {
        const char *created_at = feed["created_at"];
        const char *field1 = feed["field1"]; // humidity
        const char *field2 = feed["field2"]; // temp C
        const char *field3 = feed["field3"]; // heat index

        Serial.print("Time: ");
        Serial.println(created_at);
        Serial.print("Humidity: ");
        Serial.println(field1);
        Serial.print("Temp: ");
        Serial.println(field2);
        Serial.print("Heat index: ");
        Serial.println(field3);
        Serial.println("----");
      }
    }
    else
    {
      Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
