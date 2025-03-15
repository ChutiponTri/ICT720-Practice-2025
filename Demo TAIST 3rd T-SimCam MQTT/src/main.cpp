#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "esp_mac.h" 
#include "hw_mic.h"

#define ssid "HA_demo"
#define pass "home_assistant"
#define mqtt_broker "192.168.137.1"
#define mqtt_port 1883
#define mqtt_topic "ict720/ton/server"
#define mqtt_resp "ict720/ton/response"
#define mqtt_auth "tensai"

// Variables Declaration
WiFiClient espclient;
PubSubClient client(espclient);
SemaphoreHandle_t xSemaphore = NULL;
int mem_idx = 0;
int32_t mic_samples[2 * 1600];      // 2 Memmory index -> Allocate = 2X
JsonDocument doc;
float avg_val = 0.0;
String clientName = "Name-" + String(random(0xffff), HEX);
String chipId;

// Function Prototypes
void setup_wifi(void);
void wifi_status(void);
void setup_mqtt(void);
void mqtt_status(void);
void mqtt_reconnect(void);
void callback(char* topic, byte* payload, unsigned int length);
void read_mic_init(void);
void read_mic_task(void *pvParam);
void process_mic_init(void);
void process_mic_task(void *pvParam);
String getInterfaceMacAddress(esp_mac_type_t interface);

void setup(void) {
  Serial.begin(115200);

  // Setup WiFi
  setup_wifi();

  // Setup MQTT
  setup_mqtt();

  // Prepare Semaphore
  xSemaphore = xSemaphoreCreateBinary();

  // Create Task to Read Mic
  xTaskCreate(read_mic_task, "READ_MIC_TASK", 4096, NULL, 3, NULL);

  // Create Task to Process Mic Data
  xTaskCreate(process_mic_task, "PROCESS_MIC_TASK", 4096, NULL, 3, NULL);

  // Get Chip ID
  chipId = getInterfaceMacAddress(ESP_MAC_BT);
}

void loop(void) {
  // Loop This
  static uint8_t number = 0;
  char pub[96];
  sprintf(pub, "{\"device\":\"%s\", \"data\":%f}", chipId.c_str(), avg_val);
  mqtt_status();
  client.loop();
  client.publish(mqtt_topic, pub);
  delay(5000);
}

void setup_wifi(void) {
  WiFi.begin(ssid, pass);
  wifi_status();
}

void wifi_status(void) {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    WiFi.reconnect();
    delay(1000);
  }
  Serial.print("Connected to : ");
  Serial.println(ssid);
  Serial.print("Local IP : ");
  Serial.println(WiFi.localIP());
  Serial.print("RSSI : ");
  Serial.println(WiFi.RSSI());
}

void setup_mqtt(void) {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  client.connect(clientName.c_str());
  client.subscribe(mqtt_topic);
}

void mqtt_status(void) {
  if (!client.connected()) {
    mqtt_reconnect();
  }
}

void mqtt_reconnect(void) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientName.c_str())) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    client.publish("outTopic","hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
    } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
    delay(5000);
  }
}
client.subscribe(mqtt_topic);
}

void callback(char* topic, byte* payload, unsigned int length) {
  char buf[200];
  memcpy(buf, payload, length);
  buf[length] = '\0';
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print(buf);
  Serial.println();
  DeserializationError error = deserializeJson(doc, buf);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  if (doc["cmd"] == "ton") {
    Serial.println("Start Listening");
    doc.clear();
    doc["status"] = "ton";
    doc["value"] = avg_val;
    serializeJson(doc, buf);
    client.publish(mqtt_resp, buf);
    Serial.print(buf);
    Serial.println();
  }
}

void read_mic_init(void) {
  hw_mic_init(16000);
}

void read_mic_task(void *pvParam) {
  read_mic_init();
  int cur_mem_idx = 0;
  while(1) {
    static unsigned int num_samples = 1600;
    hw_mic_read(mic_samples + cur_mem_idx * 1600, &num_samples);
    mem_idx = cur_mem_idx;
    cur_mem_idx = (cur_mem_idx + 1) % 2;
    xSemaphoreGive(xSemaphore);
  }
}

void process_mic_init(void) {

}

void process_mic_task(void *pvParam) {
  process_mic_init();
  while(1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    avg_val = 0;
    for (int i=0; i<1600; i++){
      avg_val += (float)abs(mic_samples[mem_idx * 1600 + i]) / 1600;
    }
    // Serial.println(avg_val);
  }
}

String getInterfaceMacAddress(esp_mac_type_t interface) {

  String mac = "";

  unsigned char mac_base[6] = {0};

  if (esp_read_mac(mac_base, interface) == ESP_OK) {
    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }

  return mac;
}
