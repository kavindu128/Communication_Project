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

    SimSerial.end();                    // on a 2nd start the port is already running: stop it first
    SimSerial.setRxBufferSize(1024);    // (the buffer can only be resized while the port is stopped)

    SimSerial.begin(SIM7600_BAUD_RATE, SERIAL_8N1, SIM7600_RX, SIM7600_TX);
}
