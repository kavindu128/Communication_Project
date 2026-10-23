#ifndef SIM7080_APP_H
#define SIM7080_APP_H

#include "SIM7080_driver.h"

#define SIM7080_IDLE 0
#define SIM7080_OFF 1
#define SIM7080_STARTING 2
#define SIM7080_CONNECTING 3
#define SIM7080_CONNECTED 4

extern TaskHandle_t SIM7080AppHandle;

extern SemaphoreHandle_t SIM7080_ON_FLAG;
extern SemaphoreHandle_t SIM7080_OFF_FLAG;
extern SemaphoreHandle_t SIM7080_CONNECTED_FLAG;

extern uint8_t SIM7080_State;

void SIM7080_app_Init();
void SIM7080_app(void *pvParameters);

#endif
