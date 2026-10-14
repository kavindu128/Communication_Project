#ifndef TEST_APP_H
#define TEST_APP_H

#include <Arduino.h>
#include "SIM7600_app.h"
#include "WIFI_app.h"

extern TaskHandle_t TestHandle;

// Simple test: type a letter in the serial monitor
//   O = SIM ON    X = SIM OFF
//   W = WiFi ON   Q = WiFi OFF
//   B = BOTH ON   Z = BOTH OFF
//   G = MQTT via 4G   (starts SIM if idle; WiFi keeps sending until 4G is connected)
//   F = MQTT via WiFi (starts WiFi if idle; 4G keeps sending until WiFi is connected)
//   T = print status
void Test_app(void * pvParameters);

#endif