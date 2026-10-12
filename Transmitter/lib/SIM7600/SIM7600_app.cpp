#include "SIM7600_app.h"

TaskHandle_t SIM7600AppHandle;

SemaphoreHandle_t SIM7600_ON_FLAG = NULL;
SemaphoreHandle_t SIM7600_OFF_FLAG = NULL;
SemaphoreHandle_t SIM7600_CONNECTED_FLAG = NULL;

// Set to your local APN (e.g., "dialogbb" or "mobitel")
const char* APN = "mobitel";

uint8_t SIM7600_State = SIM7600_IDLE;

void SIM7600_app_init() {
    SIM7600_ON_FLAG = xSemaphoreCreateBinary();
    SIM7600_OFF_FLAG = xSemaphoreCreateBinary();
    SIM7600_CONNECTED_FLAG = xSemaphoreCreateBinary();
}


void SIM7600_app(void * pvParameters) {

    while (1) {
        switch (SIM7600_State) {

            case SIM7600_IDLE:
                // Modem is off. Sleep here until somebody gives the ON flag.
                if (xSemaphoreTake(SIM7600_ON_FLAG, portMAX_DELAY) == pdTRUE) {
                    xSemaphoreTake(SIM7600_OFF_FLAG, 0);   // clear the OFF flag if it was set
                    Serial.println("SIM: STARTING");
                    SIM7600_State = SIM7600_STARTING;
                }
                break;

            case SIM7600_STARTING:
                SIM7600_Driver_Init();                     // takes a while
                Serial.println("SIM: CONNECTING");
                SIM7600_State = SIM7600_CONNECTING;
                break;

            case SIM7600_CONNECTING:
                // OFF asked while we were starting or retrying?
                if (xSemaphoreTake(SIM7600_OFF_FLAG, 0) == pdTRUE) {
                    SIM7600_State = SIM7600_OFF;
                    break;
                }

                if (xSemaphoreTake(ModemMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
                    bool ok = SIM7600_Driver_ConnectGPRS(APN);
                    xSemaphoreGive(ModemMutex);

                    if (ok) {
                        Serial.println("GPRS connected successfully.");
                        Serial.println("SIM: CONNECTED");
                        SIM7600_State = SIM7600_CONNECTED;
                        break;
                    }
                }

                Serial.println("GPRS connection failed. Retrying in 5 seconds...");
                vTaskDelay(pdMS_TO_TICKS(5000));           // wait before retrying
                break;

            case SIM7600_CONNECTED:
                // The wait on the OFF flag is also our 5 second timer
                if (xSemaphoreTake(SIM7600_OFF_FLAG, pdMS_TO_TICKS(5000)) == pdTRUE) {
                    SIM7600_State = SIM7600_OFF;
                }
                else {
                    // Still online?
                    if (xSemaphoreTake(ModemMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
                        bool linkOk = SIM7600_Driver_IsConnected();
                        xSemaphoreGive(ModemMutex);

                        if (linkOk) {
                            xSemaphoreGive(SIM7600_CONNECTED_FLAG);   // other tasks use this to know we are online
                        }
                        else {
                            Serial.println("Cellular link lost. Reconnecting...");
                            Serial.println("SIM: CONNECTING");
                            SIM7600_State = SIM7600_CONNECTING;
                        }
                    }
                }
                break;

            case SIM7600_OFF:
                Serial.println("SIM: OFF");
                if (xSemaphoreTake(ModemMutex, portMAX_DELAY) == pdTRUE) {
                    SIM7600_Driver_PowerOff();
                    xSemaphoreGive(ModemMutex);
                }
                xSemaphoreTake(SIM7600_ON_FLAG, 0);        // forget an ON that was given while running
                Serial.println("SIM: IDLE");
                SIM7600_State = SIM7600_IDLE;
                break;

            default:
                SIM7600_State = SIM7600_IDLE;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}