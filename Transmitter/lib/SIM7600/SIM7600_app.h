#ifndef SiM7600_APP_H
#define SiM7600_APP_H

#include "SIM7600_driver.h"

#define SIM7600_IDLE 0
#define SIM7600_OFF 1
#define SIM7600_STARTING 2
#define SIM7600_CONNECTING 3
#define SIM7600_CONNECTED 4

extern TaskHandle_t SIM7600AppHandle;

extern SemaphoreHandle_t SIM7600_ON_FLAG;
extern SemaphoreHandle_t SIM7600_OFF_FLAG;
extern SemaphoreHandle_t SIM7600_CONNECTED_FLAG; 

extern uint8_t SIM7600_State;

void SIM7600_app_init();
void SIM7600_app(void * pvParameters);


#endif