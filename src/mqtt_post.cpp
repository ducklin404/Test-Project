// // src_B/main.cpp
// #include <Arduino.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <PubSubClient.h>
// #include "DHT.h"
// #include "secrets.h"

// // ---------- DHT B ----------
// #define DHTPIN 22   // adjust to your wiring
// #define DHTTYPE DHT11
// DHT dht(DHTPIN, DHTTYPE);

// // ---------- Topics ----------
// static const char* TOPIC_PUB_B = "lab/room/esp32B";
// static const char* TOPIC_LWT_B = "lab/room/esp32B/status";

// // ---------- TLS / MQTT ----------
// WiFiClientSecure wifiClient;
// PubSubClient mqtt(wifiClient);

// unsigned long lastPublishMs = 0;
// const unsigned long PUBLISH_INTERVAL_MS = 20000;

// static void setClock() {
//   configTime(0, 0, "pool.ntp.org", "time.nist.gov");
//   Serial.print("Syncing time (TLS) ");
//   time_t now = time(nullptr);
//   int retries = 0;
//   while (now < 8 * 3600 * 24 && retries < 50) {
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

// static void mqttConnect() {
//   mqtt.setServer(MQTT_HOST, MQTT_PORT);

//   while (!mqtt.connected()) {
//     String clientId = "esp32B-" + chipId();
//     Serial.printf("MQTT: connecting as %s ... ", clientId.c_str());

//     bool ok = mqtt.connect(
//       clientId.c_str(),
//       MQTT_USERNAME, MQTT_PASSWORD,  // required for HiveMQ Cloud
//       TOPIC_LWT_B,                   // willTopic
//       1,                             // willQoS
//       true,                          // willRetain
//       "offline"                      // willMessage
//     );

//     if (ok) {
//       Serial.println("connected");
//       mqtt.publish(TOPIC_LWT_B, "online", true);
//     } else {
//       Serial.printf("failed, rc=%d. retry in 3s\n", mqtt.state());
//       delay(3000);
//     }
//   }
// }

// static void publishB() {
//   float h = dht.readHumidity();
//   float t = dht.readTemperature();
//   if (isnan(h) || isnan(t)) {
//     Serial.println("DHT11 B read failed");
//     return;
//   }
//   char payload[96];
//   snprintf(payload, sizeof(payload), "{\"t\":%.2f,\"h\":%.2f}", t, h);

//   bool ok = mqtt.publish(TOPIC_PUB_B, payload, true); // retained
//   Serial.printf("[MQTT] B -> %s : %s (%s)\n", TOPIC_PUB_B, payload, ok ? "OK" : "FAIL");
// }

// void setup() {
//   Serial.begin(9600);
//   dht.begin();

//   wifiConnect();
//   setClock();
//   wifiClient.setCACert(HIVEMQ_CLOUD_CA_PEM);

//   mqttConnect();
// }

// void loop() {
//   if (WiFi.status() != WL_CONNECTED) wifiConnect();
//   if (!mqtt.connected()) mqttConnect();
//   mqtt.loop();

//   unsigned long now = millis();
//   if (now - lastPublishMs >= PUBLISH_INTERVAL_MS) {
//     lastPublishMs = now;
//     publishB();
//   }
// }
