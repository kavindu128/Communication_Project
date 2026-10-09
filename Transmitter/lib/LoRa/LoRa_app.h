#ifndef LORA_APP_H
#define LORA_APP_H

#include "LoRa_driver.h"
#include "Queues.h"

extern TaskHandle_t LoRaAppHandle;

void LoRa_app(void* pvParameters);

#endif // LORA_APP_H
