#ifndef SIM7600_DRIVER_H
#define SIM7600_DRIVER_H

// #define TINY_GSM_DEBUG Serial
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>

#include <Client.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "SIM7600_hal.h"

extern SemaphoreHandle_t Sim7600ModemMutex;

void SIM7600_Driver_Init();
bool SIM7600_Driver_ConnectGPRS(const char *apn);
bool SIM7600_Driver_IsConnected();
bool SIM7600_Driver_IsLinkUp();
void SIM7600_Driver_PowerOff();
void SIM7600_Driver_SetDisconnected();
Client *SIM7600_Driver_GetClient();
int SIM7600_Driver_GetSignalStrength();

#endif
