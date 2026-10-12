#ifndef SIM7600_DRIVER_H
#define SIM7600_DRIVER_H

#include "SIM7600_hal.h"
#include <Client.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdint.h>


extern SemaphoreHandle_t ModemMutex;

void SIM7600_Driver_Init();                    // power key + modem restart
bool SIM7600_Driver_ConnectGPRS(const char* apn);
bool SIM7600_Driver_IsConnected();             // asks the modem (needs the mutex)
void SIM7600_Driver_PowerOff();                // close GPRS and power the modem down
void SIM7600_Driver_SetDisconnected();
Client* SIM7600_Driver_GetClient();

#endif
