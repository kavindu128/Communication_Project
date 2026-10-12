#include "sim7600test_app.h"

TaskHandle_t SIM7600TestHandle;

void SIM7600_Test_app(void * pvParameters) {

    const char* names[] = {"IDLE", "STARTING", "CONNECTING", "CONNECTED", "OFF"};

    Serial.println("SIM7600 test ready:  O = ON   X = OFF   S = state");

    while (1) {

        if (Serial.available()) {
            char CMD = Serial.read();

            if (CMD == 'O') {
                Serial.println("Command: ON");
                xSemaphoreGive(SIM7600_ON_FLAG);
            }
            else if (CMD == 'X') {
                Serial.println("Command: OFF");
                xSemaphoreGive(SIM7600_OFF_FLAG);
            }
            else if (CMD == 'S') {
                Serial.printf("State: %s\n", names[SIM7600_State]);
                Serial.printf("SIM task stack never used: %u bytes\n",
                              (unsigned)uxTaskGetStackHighWaterMark(SIM7600AppHandle));
            }
        }

        // This is how another task (MQTT) will know the SIM is online
        if (xSemaphoreTake(SIM7600_CONNECTED_FLAG, 0) == pdTRUE) {
            Serial.println("SIM link OK");
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}