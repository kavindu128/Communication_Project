#ifndef SiM7600_APP_H
#define SiM7600_APP_H

#include "SIM7600_driver.h"

extern TaskHandle_t SIM7600AppHandle;
void SIM7600_app(void * pvParameters);

#endif