#ifndef __TELEGRAM_H__
#define __TELEGRAM_H__

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define TELEGRAM_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"

void telegram_task(void* pvParam);
void fetchTelegram(void);
void sendTelegram(String userId, String message);

extern JsonDocument doc;
extern QueueHandle_t telegramQueue;

#endif
