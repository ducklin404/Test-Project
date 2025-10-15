// #include <WiFi.h>
// #include <HTTPClient.h>
// #include "DHT.h"

// #define DHTPIN 22
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// const char* ssid = "duck";
// const char* password = "ducklin404";
// const char* serverTS = "api.thingspeak.com";
// String apiKey = "EFAAM5TKMHRAIXMW";

// WiFiServer server(80);

// float tA, hA, tB, hB;

// void setup() {
//   Serial.begin(9600);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500); Serial.print(".");
//   }
//   Serial.println("WiFi connected!");
//   Serial.print("ESP32 A IP: ");
//   Serial.println(WiFi.localIP());
//   server.begin();
//   dht.begin();
// }

// void loop() {
//   hA = dht.readHumidity();
//   tA = dht.readTemperature();
//   if (isnan(hA) || isnan(tA)) {
//     Serial.println("Failed to read from DHT11 A");
//     return;
//   }

//   WiFiClient c = server.available();
//   if (c) {
//     String line = c.readStringUntil('\n');
//     int comma = line.indexOf(',');
//     if (comma > 0) {
//       tB = line.substring(0, comma).toFloat();
//       hB = line.substring(comma + 1).toFloat();
//       Serial.printf("Received from B: T=%.2f °C, H=%.2f %%\n", tB, hB);
//     }
//     c.stop();
//   }

 

//   HTTPClient http;
//   String url = String("http://api.thingspeak.com/update?api_key=") + apiKey + "&field1=" + String(hA) + "&field2=" + String(tA) + "&field3=" + String(hB) + "&field4=" + String(tB);
//   http.begin(url);
//   int httpCode = http.GET();
//   if (httpCode == 200)
//   {
//     String payload = http.getString();
//     Serial.printf("HTTP %d, entry ID: %s\n", httpCode, payload.c_str());
//   }
//   else
//   {
//     Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
//   }

//   http.end();

//   delay(20000); 
// }