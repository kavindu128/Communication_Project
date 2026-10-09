#ifndef WIFI_APP_H
#define WIFI_APP_H

extern TaskHandle_t WiFiAppHandle;

#include "WIFI_driver.h"
void WIFI_app(void *pvParameters);

#endif // WIFI_APP_H