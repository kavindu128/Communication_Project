#ifndef SIM7600_DRIVER_H
#define SIM7600_DRIVER_H

#include "SIM7600_hal.h"
#include <Client.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdint.h>


extern SemaphoreHandle_t ModemMutex;

void SIM7600_Driver_Init();                   
bool SIM7600_Driver_ConnectGPRS(const char* apn);
bool SIM7600_Driver_IsConnected();             
bool SIM7600_Driver_IsLinkUp();                
void SIM7600_Driver_PowerOff();                
void SIM7600_Driver_SetDisconnected();
Client* SIM7600_Driver_GetClient();


#endif
