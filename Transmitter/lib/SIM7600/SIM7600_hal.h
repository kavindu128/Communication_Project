#ifndef SIM7600_HAL_H
#define SIM_7600_HAL_H

#include <Arduino.h>

#define SIM7600_RX 16
#define SIM7600_TX 17
#define SIM7600_PWRKEY 15
#define SIM7600_BAUD_RATE 115200

extern HardwareSerial& SimSerial;

void SIM7600_HAL_Init();

#endif