#include "main.h"
#include "telegram.h"

void pressureTimerCallback(TimerHandle_t xTimer) {
  int pressure = random(10, 61);                                    // สุ่มแรงดันระหว่าง 10-60 mmHg
  String pressureJson = String("{\"pressure\":") + pressure + "}";
  publish(pressureJson);
}

void telegramTimerCallback(TimerHandle_t xTimer) {
  xQueueSend(telegramQueue, "Hello Chigga", 0);
}