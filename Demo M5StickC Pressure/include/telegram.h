#ifndef __TELEGRAM_H__
#define __TELEGRAM_H__

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define TELEGRAM_TOKEN "7453097438:AAGM4fvrlrhinbZemrHwuBTHQj_7d59-02E"

void telegram_task(void* pvParam);
void fetchTelegram(void);
void sendTelegram(String userId, String message);

extern JsonDocument doc;
extern QueueHandle_t telegramQueue;

#endif