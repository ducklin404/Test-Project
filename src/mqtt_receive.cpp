// // src_A/main.cpp
// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <PubSubClient.h>
// #include "DHT.h"
// #include "secrets.h"

// // ---------- DHT A ----------
// #define DHTPIN 22
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // ---------- Topics ----------
// static const char* TOPIC_PUB_A = "lab/room/esp32A";
// static const char* TOPIC_SUB_B = "lab/room/esp32B";
// static const char* TOPIC_LWT_A = "lab/room/esp32A/status";

// // ---------- TLS / MQTT ----------
// WiFiClientSecure wifiClient;
// PubSubClient mqtt(wifiClient);

// float tA = NAN, hA = NAN;
// float tB = NAN, hB = NAN;

// unsigned long lastPublishMs = 0;
// const unsigned long PUBLISH_INTERVAL_MS = 20000;

// // ---- Helpers ----
// static void setClock() {
//   // Required for TLS certificate validation
//   configTime(0, 0, "pool.ntp.org", "time.nist.gov");
//   Serial.print("Syncing time (TLS) ");
//   time_t now = time(nullptr);
//   int retries = 0;
//   while (now < 8 * 3600 * 24 && retries < 50) { // roughly > 1970-01-09
//     delay(200);
//     Serial.print(".");
//     now = time(nullptr);
//     retries++;
//   }
//   Serial.println();
// }

// static String chipId() {
//   uint64_t mac = ESP.getEfuseMac();
//   char id[17];
//   snprintf(id, sizeof(id), "%08X%08X", (uint32_t)(mac >> 32), (uint32_t)mac);
//   return String(id);
// }

// static void wifiConnect() {
//   if (WiFi.status() == WL_CONNECTED) return;
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("WiFi: connecting");
//   while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
//   Serial.printf("\nWiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
// }

// static void onMqttMessage(char* topic, byte* payload, unsigned int len) {
//   String msg; msg.reserve(len);
//   for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];

//   // Prefer JSON {"t":..,"h":..}, else CSV "t,h"
//   int ti = msg.indexOf("\"t\"");
//   int hi = msg.indexOf("\"h\"");
//   if (ti != -1 && hi != -1) {
//     int ct = msg.indexOf(':', ti);
//     int cm = msg.indexOf(',', ct);
//     int ch = msg.indexOf(':', hi);
//     int rb = msg.indexOf('}', ch);
//     if (ct != -1 && cm != -1 && ch != -1 && rb != -1) {
//       tB = msg.substring(ct + 1, cm).toFloat();
//       hB = msg.substring(ch + 1, rb).toFloat();
//       Serial.printf("[MQTT] B JSON -> T=%.2f°C, H=%.2f%%\n", tB, hB);
//       return;
//     }
//   }
//   int comma = msg.indexOf(',');
//   if (comma > 0) {
//     tB = msg.substring(0, comma).toFloat();
//     hB = msg.substring(comma + 1).toFloat();
//     Serial.printf("[MQTT] B CSV  -> T=%.2f°C, H=%.2f%%\n", tB, hB);
//   } else {
//     Serial.printf("[MQTT] Unrecognized payload on %s: %s\n", topic, msg.c_str());
//   }
// }

// static void mqttConnect() {
//   mqtt.setServer(MQTT_HOST, MQTT_PORT);
//   mqtt.setCallback(onMqttMessage);

//   while (!mqtt.connected()) {
//     String clientId = "esp32A-" + chipId();
//     Serial.printf("MQTT: connecting as %s ... ", clientId.c_str());

//     // Use connect() overload that sets the WILL (since setWill() is not in PubSubClient)
//     bool ok = mqtt.connect(
//       clientId.c_str(),
//       MQTT_USERNAME, MQTT_PASSWORD,   // required for HiveMQ Cloud
//       TOPIC_LWT_A,                    // willTopic
//       1,                              // willQoS
//       true,                           // willRetain
//       "offline"                       // willMessage
//     );

//     if (ok) {
//       Serial.println("connected");
//       mqtt.publish(TOPIC_LWT_A, "online", true);
//       mqtt.subscribe(TOPIC_SUB_B, 1);
//     } else {
//       Serial.printf("failed, rc=%d. retry in 3s\n", mqtt.state());
//       delay(3000);
//     }
//   }
// }

// static void publishA() {
//   hA = dht.readHumidity();
//   tA = dht.readTemperature(); // °C
//   if (isnan(hA) || isnan(tA)) {
//     Serial.println("DHT11 A read failed");
//     return;
//   }
//   char payload[160];
//   snprintf(payload, sizeof(payload),
//            "{\"device\":\"esp32A\",\"t\":%.2f,\"h\":%.2f,\"tB\":%s,\"hB\":%s}",
//            tA, hA,
//            isnan(tB) ? "null" : String(tB, 2).c_str(),
//            isnan(hB) ? "null" : String(hB, 2).c_str());

//   bool ok = mqtt.publish(TOPIC_PUB_A, payload, true); // retained
//   Serial.printf("[MQTT] A -> %s : %s (%s)\n", TOPIC_PUB_A, payload, ok ? "OK" : "FAIL");
// }

// void setup() {
//   Serial.begin(9600);
//   dht.begin();

//   wifiConnect();
//   setClock();                               // TLS needs correct time
//   wifiClient.setCACert(HIVEMQ_CLOUD_CA_PEM); // set CA before connecting

//   mqttConnect();
// }

// void loop() {
//   if (WiFi.status() != WL_CONNECTED) wifiConnect();
//   if (!mqtt.connected()) mqttConnect();
//   mqtt.loop();

//   unsigned long now = millis();
//   if (now - lastPublishMs >= PUBLISH_INTERVAL_MS) {
//     lastPublishMs = now;
//     publishA();
//   }
// }
