#ifndef SIM7080_DRIVER_H
#define SIM7080_DRIVER_H

#include "SIM7080_hal.h"

extern SemaphoreHandle_t Sim7080ModemMutex;

void SIM7080_Driver_Init();
bool SIM7080_Driver_ConnectGPRS(const char* apn);
bool SIM7080_Driver_IsConnected();
bool SIM7080_Driver_IsLinkUp();
void SIM7080_Driver_PowerOff();
void SIM7080_Driver_SetDisconnected();
Client* SIM7080_Driver_GetClient();

#endif