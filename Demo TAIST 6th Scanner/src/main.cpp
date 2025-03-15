#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define ssid "EZEKIEL"
#define pass "late1978"
#define mqtt_broker "192.168.137.1"
#define mqtt_port 1883
#define mqtt_beat "ict720/ton/esp32/beat"
#define mqtt_topic "ict720/ton/esp32/data"
#define mqtt_cmd "ict720/ton/esp32/cmd"

// function prototypes
void on_message(char* topic, byte* payload, unsigned int length);

// shared variables
uint32_t scanTime = 3;  
BLEScan *pBLEScan;
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
JsonDocument doc;
String clientName = "Grace" + String(random(0xffff), HEX);

// callback for BLE scan
class CustomAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
  }
};

void setup() {
  // 1. init WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());

  // 2. init BLE scanner
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new CustomAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // 3. connect to MQTT broker
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(on_message);
  mqtt_client.connect(clientName.c_str());
  mqtt_client.subscribe(mqtt_cmd);
  Serial.println("Connected to MQTT broker");
}

void loop() {
  char payload[100];
  doc.clear();
  doc["millis"] = millis();
  doc["mac"] = WiFi.macAddress().c_str();
  doc["rssi"] = WiFi.RSSI();
  doc["ip"] = WiFi.localIP().toString().c_str();
  serializeJson(doc, payload);
  mqtt_client.publish(mqtt_beat, payload);
  mqtt_client.loop();
  // scan for BLE devices
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.printf("Found %d devices\n", foundDevices.getCount());
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (device.haveName()) {
      if (strncmp(device.getName().c_str(), "Ton-M5StickC", strlen("Ton-M5StickC")) == 0) {
        Serial.printf("Found Asset: %s\n", device.getName().c_str());
        doc.clear();
        doc["millis"] = millis();
        doc["name"] = device.getName().c_str();
        doc["mac"] = device.getAddress().toString().c_str();
        doc["rssi"] = device.getRSSI();
        serializeJson(doc, payload);
        mqtt_client.publish(mqtt_topic, payload);
        mqtt_client.loop();
      } else {
        Serial.printf("Found device: %s\n", device.getName().c_str());
      }
    } else {
      Serial.printf("Device without name: %s\n", device.getAddress().toString().c_str());
    }
  }
}

void on_message(char* topic, byte* payload, unsigned int length) {
  // char buf[200];
  // memcpy(buf, payload, length);
  // buf[length] = '\0';
  // Serial.printf("Received message from topic %s: %s\n", topic, buf);
  // deserializeJson(doc, buf);
  // if (doc["cmd"] == "listen") {
  //   // do something
  //   Serial.println("Start listening");
  //   doc.clear();
  //   doc["status"] = "ok";
  //   doc["value"] = avg_val;
  //   serializeJson(doc, buf);
  //   mqtt_client.publish("ict720/supachai/esp32/resp", buf);
  // }
}