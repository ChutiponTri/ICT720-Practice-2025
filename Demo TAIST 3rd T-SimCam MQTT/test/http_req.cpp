#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

#define ssid "EZEKIEL"
#define pass "late1978"
#define mqtt_broker "broker.emqx.io"
#define mqtt_port 1883
#define mqtt_topic "ton/server/test"
#define mqtt_auth "tensai"

WiFiClient espclient;
PubSubClient client(espclient);
char buf[96];

void setup_wifi(void);
void wifi_status(void);
void sendToTelegram(String message);

void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
}

void loop(void) {
  // put your main code here, to run repeatedly:
  wifi_status();
  static uint8_t number = 0;
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "Number = %d", number);
  if (number % 10 == 0) {
    sendToTelegram(String(number));
  }
  delay(5000);
}

void setup_wifi(void) {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifi_status(void) {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
}
 
void sendToTelegram(String message) {
  const char* host = "api.telegram.org";
  const char* botToken = "YOUR_BOT_TOKEN";  // Replace with your bot token
  const char* chatID = "7086001303";      // Replace with your chat ID
  
  WiFiClientSecure http_client;
  Serial.println("Try to send Telegram message");

  http_client.setInsecure();  // Use for HTTPS without certificate validation
  if (!http_client.connect(host, 443)) {
    Serial.println("Telegram Connection failed");
    return;
  }
  Serial.println("Telegram Connected");

  // Construct the URL path
  String url = "/bot" + String(botToken) + "/sendMessage";

  // Create the JSON body
  String postData = "{\"chat_id\":\"" + String(chatID) + "\",\"text\":\"" + message + "\"}";

  // Create the HTTP request
  String request = String("") +
                "POST " + url + " HTTP/1.1\r\n" +
                "Host: " + String(host) + "\r\n" + 
                "Content-Type: application/json\r\n" +
                "Content-Length: " + String(postData.length()) + "\r\n\r\n" +
                postData + "\r\n";

  http_client.print(request);

  // Wait for response
  while (http_client.connected()) {
    String line = http_client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // Read response body
  String response = http_client.readStringUntil('\n');
  Serial.println("Response: " + response);
}
