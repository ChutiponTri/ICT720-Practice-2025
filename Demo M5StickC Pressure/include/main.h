#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include <M5StickC.h>

void publish(String pressureJsonString);
void pressureTimerCallback(TimerHandle_t xTimer);
void telegramTimerCallback(TimerHandle_t xTimer);

#endif