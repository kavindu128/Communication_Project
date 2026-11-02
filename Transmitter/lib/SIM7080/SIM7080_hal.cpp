#include "SIM7080_hal.h"

HardwareSerial &Sim7080Serial = Serial1;

void powerOnSIM7080G()
{
    pinMode(SIM7080_PWRKEY, OUTPUT);

    digitalWrite(SIM7080_PWRKEY, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));

    digitalWrite(SIM7080_PWRKEY, LOW);
    vTaskDelay(pdMS_TO_TICKS(1200));

    digitalWrite(SIM7080_PWRKEY, HIGH);

    vTaskDelay(pdMS_TO_TICKS(5000));
}

void SIM7080_HAL_Init()
{
    powerOnSIM7080G();
    Sim7080Serial.end();
    Sim7080Serial.setRxBufferSize(1024);

    Sim7080Serial.begin(SIM7080_BOUND_RATE, SERIAL_8N1, SIM7080_RX, SIM7080_TX);
}