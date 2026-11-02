#ifndef LORA_APP_H
#define LORA_APP_H

#include "LoRa_driver.h"
#include "Queues.h"

#define LORA_IDLE 0
#define LORA_STARTING 1
#define LORA_READY 2
#define LORA_OFF 3          
#define LORA_ERROR 4

extern TaskHandle_t LoRaAppHandle;

extern SemaphoreHandle_t LoRa_ON_FLAG;
extern SemaphoreHandle_t LoRa_OFF_FLAG;
extern SemaphoreHandle_t LoRa_READY_FLAG;

extern uint8_t LoRa_State;

void LoRa_app_Init(void);
void LoRa_app(void *pvParameters);

#endif // LORA_APP_H
