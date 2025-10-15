// #include <WiFi.h>
// #include <HTTPClient.h>
// #include "DHT.h"

// #define DHTPIN 22
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // ---- UART pins for Serial2 on A ----
// #define A_RX 16   // connect to B_TX
// #define A_TX 17   // connect to B_RX
// #define UART_BAUD 115200

// const char* ssid = "duck";
// const char* password = "ducklin404";
// String apiKey = "EFAAM5TKMHRAIXMW";

// float tA, hA, tB = NAN, hB = NAN;

// void setup() {
//   Serial.begin(9600);               // USB debug
//   Serial2.begin(UART_BAUD, SERIAL_8N1, A_RX, A_TX); // UART to B

//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500); Serial.print(".");
//   }
//   Serial.println("\nWiFi connected!");
//   Serial.print("ESP32 A IP: ");
//   Serial.println(WiFi.localIP());

//   dht.begin();
// }

// void loop() {
//   // --- Read own DHT (A) ---
//   hA = dht.readHumidity();
//   tA = dht.readTemperature();
//   if (isnan(hA) || isnan(tA)) {
//     Serial.println("Failed to read from DHT11 A");
//     delay(2000);
//     return;
//   }

//   // --- Read one line from UART (from B) if available ---
//   if (Serial2.available()) {
//     String line = Serial2.readStringUntil('\n'); // waits up to default timeout (1s)
//     line.trim();
//     int comma = line.indexOf(',');
//     if (comma > 0) {
//       tB = line.substring(0, comma).toFloat();
//       hB = line.substring(comma + 1).toFloat();
//       Serial.printf("UART from B: T=%.2f °C, H=%.2f %%\n", tB, hB);
//     } else {
//       Serial.printf("Bad UART line: '%s'\n", line.c_str());
//     }
//   }

//   // --- Upload to ThingSpeak (uses A's Wi-Fi) ---
//   HTTPClient http;
//   String url = String("http://api.thingspeak.com/update?api_key=") + apiKey +
//                "&field1=" + String(hA) +
//                "&field2=" + String(tA) +
//                "&field3=" + (isnan(hB) ? "" : String(hB)) +
//                "&field4=" + (isnan(tB) ? "" : String(tB));
//   http.begin(url);
//   int httpCode = http.GET();
//   if (httpCode == 200) {
//     String payload = http.getString();
//     Serial.printf("HTTP %d, entry ID: %s\n", httpCode, payload.c_str());
//   } else {
//     Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
//   }
//   http.end();

//   delay(20000); // ThingSpeak min ~15s; 20s is safe
// }
