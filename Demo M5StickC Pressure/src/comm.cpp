#include "comm.h"

// ----------- CONFIG ----------- //
const char* ssid = "EZEKIEL";
const char* password = "late1978";
const char* mqtt_server = "broker.emqx.io"; // หรือใช้ IP/hostname ของคุณ
 
const char* device_name = "StabilizerDevice"; // ชื่ออุปกรณ์ของคุณ
const char* pair_topic = "stabilizer/server/pair";
const char* dev_topic = "stabilizer/server/device";

char data_topic[100];
char cmd_topic[100];
 
WiFiClient espClient;
PubSubClient client(espClient);

// Function to Connect WiFi 
void setup_wifi(void) {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}
 
// Function to Connect MQTT 
void setup_mqtt(void) {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe(pair_topic);
  client.subscribe(cmd_topic);
}

// Reconnect MQTT Function
void reconnect(void) {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(device_name)) {
      Serial.println("connected");
      client.subscribe(pair_topic);
      client.subscribe(cmd_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// MQTT Callback Function
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // ปิดข้อความให้ปลอดภัย
  String msg = String((char*)payload);
 
  if (String(topic) == pair_topic && msg.indexOf("Pair Request") >= 0) {
    String response = String("{\"device\":\"") + device_name + "\"}";
    client.publish(dev_topic, response.c_str());
    Serial.println("Responded to Pair Request");
  } else if (String(topic) == cmd_topic) {
    Serial.print("Received CMD: ");
    Serial.println(msg);
    // สามารถแปลง JSON เพื่อควบคุมแรงดันหรือโหมดได้ที่นี่
  }
}

// MQTT Publish Function
void publish(String pressureJsonString) {
  client.publish(data_topic, pressureJsonString.c_str());
  Serial.println("Published: " + pressureJsonString);
}
 
