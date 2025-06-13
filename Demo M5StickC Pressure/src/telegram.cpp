#include "telegram.h"

JsonDocument doc;
QueueHandle_t telegramQueue;

// Function to run Telegram Task
void telegram_task(void* pvParam){
  String message;
  while(1) {
    xQueueReceive(telegramQueue, &message, portMAX_DELAY);
    Serial.println(message.c_str());
    fetchTelegram();
  }
}

// Function to Fetch Update from Telegram
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

// Function to Send Update to Telegram
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