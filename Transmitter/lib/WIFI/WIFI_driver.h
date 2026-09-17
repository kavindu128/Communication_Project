#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#define WIFI_SSID       "ESP32_SENSOR"
#define WIFI_PASSWORD   "12345678"
#define WIFI_PORT       5000

#include <Arduino.h>

bool WIFI_Init();
bool WIFI_SendData(const uint8_t *data, size_t len);
bool WIFI_ReceiveData(uint8_t *data, size_t len);

#endif // WIFI_DRIVER_H