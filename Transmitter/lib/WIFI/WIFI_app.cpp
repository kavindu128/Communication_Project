#include "WIFI_driver.h"
#include "WIFI_app.h"
#include "WIFI_hal.h" 

TaskHandle_t WiFiAppHandle;


SemaphoreHandle_t WIFI_ON_FLAG = NULL;
SemaphoreHandle_t WIFI_OFF_FLAG = NULL;
SemaphoreHandle_t WIFI_CONNECTED_FLAG = NULL;

uint8_t WIFI_State = WIFI_STATE_IDLE;

void WIFI_app_init() {
    WIFI_ON_FLAG = xSemaphoreCreateBinary();
    WIFI_OFF_FLAG = xSemaphoreCreateBinary();
    WIFI_CONNECTED_FLAG = xSemaphoreCreateBinary();
}



void WIFI_app(void *pvParameters) {

    bool attemptActive = false;     // true while one WiFi.begin() attempt is running
    uint32_t attemptStart = 0;

    while (1) {
        switch (WIFI_State) {
            case WIFI_STATE_IDLE:
                // WiFi is off. Sleep here until somebody gives the ON flag.
                if (xSemaphoreTake(WIFI_ON_FLAG, portMAX_DELAY) == pdTRUE) {
                    xSemaphoreTake(WIFI_OFF_FLAG, 0);   // clear the OFF flag if it was set
                    Serial.println("WiFi: STARTING");
                    WIFI_State = WIFI_STATE_STARTING;
                }
                break;

            case WIFI_STATE_STARTING:
                WIFI_Driver_Init();                     // takes a while
                Serial.println("WiFi: CONNECTING");
                WIFI_State = WIFI_STATE_CONNECTING;
                attemptActive = false;                  // reset for new connection attempt
                break;

            case WIFI_STATE_CONNECTING:
                // start one attempt
                if (!attemptActive) {
                    WIFI_Driver_Connect(WIFI_SSID, WIFI_PASSWORD);
                    attemptStart = millis();
                    attemptActive = true;
                }

                // The wait on the OFF flag is also our 500 ms poll timer,
                // so OFF is answered quickly even while connecting.
                if (xSemaphoreTake(WIFI_OFF_FLAG, pdMS_TO_TICKS(500)) == pdTRUE) {
                    WIFI_State = WIFI_STATE_OFF;
                    break;
                }

                if (WIFI_Driver_IsConnected()) {
                    WIFI_Driver_PrintIP();
                    attemptActive = false;
                    Serial.println("WIFI: CONNECTED");
                    WIFI_State = WIFI_STATE_CONNECTED;
                }
                else if (millis() - attemptStart > 15000) {
                    Serial.println("WiFi connection failed. Retrying in 5 seconds...");
                    WIFI_Driver_Disconnect();
                    attemptActive = false;
                    // wait before retrying, but still listen for OFF
                    if (xSemaphoreTake(WIFI_OFF_FLAG, pdMS_TO_TICKS(5000)) == pdTRUE) {
                        WIFI_State = WIFI_STATE_OFF;
                    }
                }
                break;

            case WIFI_STATE_CONNECTED:
                // The wait on the OFF flag is also our 1 second timer
                if (xSemaphoreTake(WIFI_OFF_FLAG, pdMS_TO_TICKS(1000)) == pdTRUE) {
                    WIFI_State = WIFI_STATE_OFF;
                }
                else if (WIFI_Driver_IsConnected()) {
                    xSemaphoreGive(WIFI_CONNECTED_FLAG);   // other tasks use this to know we are online
                }
                else {
                    Serial.println("WiFi connection lost. Reconnecting...");
                    Serial.println("WIFI: CONNECTING");
                    attemptActive = false;
                    WIFI_State = WIFI_STATE_CONNECTING;
                }
                break;

             case WIFI_STATE_OFF:
                Serial.println("WIFI: OFF");
                WIFI_Driver_PowerOff();
                attemptActive = false;
                xSemaphoreTake(WIFI_ON_FLAG, 0);           // forget an ON that was given while running
                Serial.println("WIFI: IDLE");
                WIFI_State = WIFI_STATE_IDLE;
                break;

            default:
                WIFI_State = WIFI_STATE_IDLE;
            }

        vTaskDelay(pdMS_TO_TICKS(100));

    }
    
}


    
