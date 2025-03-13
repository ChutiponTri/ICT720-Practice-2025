#include <Arduino.h>
#include "hw_mic.h"

#define TAG "main"

void setup(void) {
  Serial.begin(115000);
  hw_mic_init(16000);
}

void loop(void) {
  static int32_t buf[1600];
  static uint32_t num_samples = 1600;
  float avg_val = 0;

  hw_mic_read(buf, &num_samples);
  for (int i=0; i< num_samples; i++){
    avg_val += (float)abs(buf[i]) / num_samples;
  }

  Serial.printf("%d, %f\n", num_samples,avg_val);
}
