#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <Arduino.h>

bool WIFI_Init();
bool WIFI_SendData(const uint8_t *data, size_t len);
bool WIFI_ReceiveData(uint8_t *data, size_t len);

#endif // WIFI_DRIVER_H