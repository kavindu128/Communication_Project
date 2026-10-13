#ifndef WIFI_APP_H
#define WIFI_APP_H

#include "WIFI_driver.h"

extern TaskHandle_t WiFiAppHandle;

#define WIFI_STATE_IDLE        0
#define WIFI_STATE_OFF         1
#define WIFI_STATE_STARTING    2
#define WIFI_STATE_CONNECTING  3
#define WIFI_STATE_CONNECTED   4

extern SemaphoreHandle_t WIFI_ON_FLAG;
extern SemaphoreHandle_t WIFI_OFF_FLAG;
extern SemaphoreHandle_t WIFI_CONNECTED_FLAG;

extern uint8_t WIFI_State;

void WIFI_app_init();
void WIFI_app(void *pvParameters);

#endif // WIFI_APP_H