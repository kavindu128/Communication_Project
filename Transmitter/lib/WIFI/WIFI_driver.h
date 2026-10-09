#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>

void WIFI_Driver_Init();
void WIFI_Driver_Connect(const char* ssid, const char* password);
bool WIFI_Driver_IsConnected();
Client* WIFI_Driver_GetClient();
void WIFI_Driver_PrintIP();


#endif // WIFI_DRIVER_H