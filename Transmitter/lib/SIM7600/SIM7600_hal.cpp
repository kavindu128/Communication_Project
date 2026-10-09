#include "SIM7600_hal.h"

HardwareSerial& SimSerial = Serial2;

static void powerOnSIM7600G() {
    pinMode(SIM7600_PWRKEY, OUTPUT);

    digitalWrite(SIM7600_PWRKEY, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1200));

    digitalWrite(SIM7600_PWRKEY, LOW);
    vTaskDelay(pdMS_TO_TICKS(1200));

    digitalWrite(SIM7600_PWRKEY, HIGH);

    vTaskDelay(pdMS_TO_TICKS(10000));
}

void SIM7600_HAL_Init() {
    powerOnSIM7600G();
    
    SimSerial.setRxBufferSize(1024); 
    SimSerial.begin(SIM7600_BAUD_RATE, SERIAL_8N1, SIM7600_RX, SIM7600_TX);
}
