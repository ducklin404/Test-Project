#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

// ---------- DHT setup ----------
#define DHTPIN 22
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LED_PIN 2

// ---------- WiFi ----------
const char* ssid     = "duck";
const char* password = "ducklin404";

// ---------- ThingsBoard MQTT ----------
const char* tb_host = "eu.thingsboard.cloud";  
const uint16_t tb_port = 1883;            
const char* tb_token = "biYowwlZy4ehUGPnctlQ"; 

const bool isAuto = false;
const double threshhold = 20;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

WiFiServer server(80);
float tA = NAN, hA = NAN; 

// ---------- Helpers ----------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void rpcCallback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.f_str());
    return;
  }

  const char* method = doc["method"];
  bool state = doc["params"];

  Serial.println(method);
  Serial.println(state);

  if (strcmp(method, "setLed") == 0) {
    digitalWrite(LED_PIN, state ? HIGH : LOW);  
    Serial.println(state ? "LED ON" : "LED OFF");
  }

  else if (strcmp(method, "setAuto") == 0){
    
  } 

}

void ensureMqttConnected() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to ThingsBoard MQTT...");
    if (mqtt.connect("esp32-client", tb_token, nullptr)) {
      Serial.println("connected.");

      mqtt.subscribe("v1/devices/me/rpc/request/+");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" retry in 2s");
      delay(2000);
    }
  }
}

void publishTelemetry(float tA, float hA) {
  String payload = "{";
  bool first = true;

  if (!isnan(tA)) { if (!first) payload += ","; payload += "\"temperature\":" + String(tA, 2); first = false; }
  if (!isnan(hA)) { if (!first) payload += ","; payload += "\"humid\":"  + String(hA, 2); first = false; }

  payload += "}";

  const char* topic = "v1/devices/me/telemetry";
  bool ok = mqtt.publish(topic, payload.c_str(), /*retained=*/false);
  if (ok) {
    Serial.print("Published telemetry: ");
    Serial.println(payload);
  } else {
    Serial.println("Publish failed!");
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);          
  digitalWrite(LED_PIN, HIGH);       

  connectWiFi();
  mqtt.setServer(tb_host, tb_port);
  mqtt.setCallback(rpcCallback);
  server.begin();
}

unsigned long lastSend = 0;
const unsigned long sendIntervalMs = 20000;

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  ensureMqttConnected();
  mqtt.loop(); // keep MQTT alive

  // ---- Read DHT11 (device A) ----
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h)) hA = h;
  if (!isnan(t)) tA = t;
 

  // ---- Publish periodically ----
  unsigned long now = millis();
  if (now - lastSend >= sendIntervalMs) {
    publishTelemetry(tA, hA);
    lastSend = now;
  }
}
