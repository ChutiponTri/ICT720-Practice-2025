#include "main.h"
#include "comm.h"
#include "telegram.h"

TimerHandle_t pressureTimer;
TimerHandle_t telegramTimer;

// Setup Function
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

  telegramQueue = xQueueCreate(10, sizeof(String));
  xTaskCreate(telegram_task, "TELEGRAM_TASK", 4096, NULL, 1, NULL);

  pressureTimer = xTimerCreate("PRESSURE_TIMER", pdMS_TO_TICKS(1000), pdTRUE, NULL, pressureTimerCallback);
  telegramTimer = xTimerCreate("TELEGRAM_TIMER", pdMS_TO_TICKS(5000), pdTRUE, NULL, telegramTimerCallback);
  xTimerStart(pressureTimer, 0);
  xTimerStart(telegramTimer, 0);
 
  snprintf(data_topic, sizeof(data_topic), "stabilizer/server/data/%s", device_name);
  snprintf(cmd_topic, sizeof(cmd_topic), "stabilizer/server/cmd/%s", device_name);
 
  setup_wifi();
  setup_mqtt();
}
 
// Loop Function
void loop(void) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
 
