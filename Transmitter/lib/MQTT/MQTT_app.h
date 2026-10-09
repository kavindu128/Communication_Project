#ifndef MQTT_APP_H
#define MQTT_APP_H

#include "MQTT_driver.h"

extern TaskHandle_t MQTTAppHandle;

void MQTT_app(void *pvParameters);

#endif // MQTT_APP_H