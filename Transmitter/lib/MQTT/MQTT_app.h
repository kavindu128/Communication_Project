#ifndef MQTT_APP_H
#define MQTT_APP_H

#include "MQTT_driver.h"

extern TaskHandle_t MQTTAppHandle;
extern volatile MQTT_Transport MQTT_App_BoundTransport;
extern volatile bool MQTT_App_Connected;

void MQTT_App_ProcessCommand(const char* method, bool state);
void MQTT_app(void *pvParameters);

#endif // MQTT_APP_H