#include <Arduino.h>
#include <M5StickC.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
 
// ----------- CONFIG ----------- //
#define TELEGRAM_TOKEN "YOUR_TELEGRAM_TOKEN"

void setup_wifi(void);
void setup_mqtt(void);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect(void);
void fetchTelegram(void);
void sendTelegram(String userId, String message);

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
JsonDocument doc;

// ----------- SETUP ----------- // 
void setup(void) {
  Serial.begin(115200);
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(GREEN);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(40, 25);
  M5.Lcd.println("TON");
 
  snprintf(data_topic, sizeof(data_topic), "stabilizer/server/data/%s", device_name);
  snprintf(cmd_topic, sizeof(cmd_topic), "stabilizer/server/cmd/%s", device_name);
 
  setup_wifi();
  setup_mqtt();
}
 
// ----------- LOOP ----------- //
void loop(void) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  // สุ่มค่าแรงดันและส่งทุก ๆ 1 วินาที
  static unsigned long lastSent = 0;
  if (millis() - lastSent > 1000) {
    int pressure = random(10, 61); // สุ่มแรงดันระหว่าง 10-60 mmHg
    String pressureJson = String("{\"pressure\":") + pressure + "}";
    client.publish(data_topic, pressureJson.c_str());
    Serial.println("Published: " + pressureJson);
    lastSent = millis();

    fetchTelegram();
    // sendTelegram("Hello Heat Guy");
  }
}
 
// ----------- MQTT Callback ----------- //
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
 
// ----------- Connect WiFi ----------- //
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
 
// ----------- Connect MQTT ----------- //
void setup_mqtt(void) {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe(pair_topic);
  client.subscribe(cmd_topic);
}

// ---------- Reconnect MQTT ---------- //
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

// ---------- Fetch Update from Telegram ---------- //
void fetchTelegram(void) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TELEGRAM_TOKEN) + "/getUpdates";
  http.begin(url);  

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();

    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.println("Error:");
      Serial.println(error.c_str());
    } else {
      JsonArray results = doc["result"].as<JsonArray>();
      JsonObject lastMessage = results[results.size() - 1];
      String userId = lastMessage["message"]["chat"]["id"];
      String message = lastMessage["message"]["text"];
      
      Serial.println("User Id:");
      Serial.println(userId.c_str());

      sendTelegram(userId, message);
    }

  } else {
    Serial.print("GET failed: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

// ---------- Send Update to Telegram ---------- //
void sendTelegram(String userId, String message) {
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(TELEGRAM_TOKEN) + "/sendMessage";
  String payload = "{\"chat_id\":\"" + userId + "\",\"text\":\"" + message + "\"}";

  http.begin(url); 
  http.addHeader("Content-Type", "application/json");  // Header Config

  int httpCode = http.POST(payload);                   // POST Request with payload
  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.print("POST failed: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

