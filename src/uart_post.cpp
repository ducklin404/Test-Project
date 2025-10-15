// #include "DHT.h"

// // ---- DHT on B ----
// #define DHTPIN 21
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // ---- UART pins for Serial2 on B ----
// #define B_RX 4     // connect to A_TX
// #define B_TX 5     // connect to A_RX
// #define UART_BAUD 115200

// void setup() {
//   Serial.begin(9600); // USB debug (optional)
//   Serial2.begin(UART_BAUD, SERIAL_8N1, B_RX, B_TX); // UART to A
//   dht.begin();
// }

// void loop() {
//   float h = dht.readHumidity();
//   float t = dht.readTemperature();
//   if (isnan(h) || isnan(t)) {
//     Serial.println("Failed to read from DHT11 B");
//   } else {
//     // Send "temp,hum\n"
//     Serial2.print(t, 2); Serial2.print(",");
//     Serial2.print(h, 2); Serial2.print("\n");
//     Serial.printf("Sent to A: T=%.2f, H=%.2f\n", t, h);
//   }
//   delay(5000);
// }
