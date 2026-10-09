#ifndef SIM7600_DRIVER_H
#define SIM7600_DRIVER_H

#include "SIM7600_hal.h"
#include <Client.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t ModemMutex;

void SIM7600_Driver_Init();
bool SIM7600_Driver_ConnectGPRS(const char* apn);
bool SIM7600_Driver_IsConnected();
void SIM7600_Driver_SetDisconnected();
void SIM7600_Driver_Poll();
Client* SIM7600_Driver_GetClient();

#endif
