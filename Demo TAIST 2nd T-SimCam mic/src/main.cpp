#include <Arduino.h>
#include <task.h>
#include <queue.h>
#include "hw_mic.h"

// As T-Simcam has 2 mics (Left and Right), so use multitasking to choose which one to read

// Variables Declaration
SemaphoreHandle_t xSemaphore = NULL;
int mem_idx = 0;
int32_t mic_samples[2 * 1600];      // 2 Memmory index -> Allocate = 2X

// Function Prototypes
void read_mic_init(void);
void read_mic_task(void *pvParam);
void process_mic_init(void);
void process_mic_task(void *pvParam);

void setup(void) {
  // 1. Prepare Semaphore
  xSemaphore = xSemaphoreCreateBinary();

  // 2. Create Task to Read Mic
  xTaskCreate(read_mic_task, "READ_MIC_TASK", 4096, NULL, 3, NULL);

  // 3. Create Task to Process Mic Data
  xTaskCreate(process_mic_task, "PROCESS_MIC_TASK", 4096, NULL, 3, NULL);
}

void loop(void) {
  delay(100);
}

void read_mic_init(void) {
  hw_mic_init(16000);
}

void read_mic_task(void *pvParam) {
  read_mic_init();
  int cur_mem_idx = 0;
  while(1) {
    static unsigned int num_samples = 1600;
    hw_mic_read(mic_samples + cur_mem_idx * 1600, &num_samples);
    mem_idx = cur_mem_idx;
    cur_mem_idx = (cur_mem_idx + 1) % 2;
    xSemaphoreGive(xSemaphore);
  }
}

void process_mic_init(void) {
 Serial.begin(115200);
}

void process_mic_task(void *pvParam) {
  process_mic_init();
  while(1) {
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    float avg_val = 0.0;
    for (int i=0; i<1600; i++){
      avg_val += (float)abs(mic_samples[mem_idx * 1600 + i]) / 1600;
    }
    Serial.println(avg_val);
  }
}
