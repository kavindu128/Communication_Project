#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <stdint.h>

bool WIFI_Init();



bool WIFI_ReceiveData(
    uint8_t *data,
    size_t len
);

#endif