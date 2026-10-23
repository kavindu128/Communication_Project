#ifndef SIM7080_HAL_H
#define SIM7080_HAL_H

#include <Arduino.h>

extern HardwareSerial& Sim7080Serial;

#define SIM7080_RX 4
#define SIM7080_TX 5
#define SIM7080_PWRKEY 1
#define SIM7080_BOUND_RATE 115200

void powerOnSIM7080G();
void SIM7080_HAL_Init();

#endif