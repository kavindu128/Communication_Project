#ifndef MQTT_HAL_H
#define MQTT_HAL_H

#include <Client.h>

void MQTT_HAL_Init();

Client* MQTT_HAL_GetActiveClient();

#endif // MQTT_HAL_H