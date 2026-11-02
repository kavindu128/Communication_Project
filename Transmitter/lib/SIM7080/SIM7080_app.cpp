#include "SIM7080_app.h"

TaskHandle_t SIM7080AppHandle;

SemaphoreHandle_t SIM7080_ON_FLAG = NULL;
SemaphoreHandle_t SIM7080_OFF_FLAG = NULL;
SemaphoreHandle_t SIM7080_CONNECTED_FLAG = NULL;

const char *SIM7080_APN = "nbiot";
uint8_t SIM7080_State = SIM7080_IDLE;

void SIM7080_app_Init()
{
    SIM7080_ON_FLAG = xSemaphoreCreateBinary();
    SIM7080_OFF_FLAG = xSemaphoreCreateBinary();
    SIM7080_CONNECTED_FLAG = xSemaphoreCreateBinary();

    if (Sim7080ModemMutex == NULL)
    {
        Sim7080ModemMutex = xSemaphoreCreateMutex();
    }
}

void SIM7080_app(void *pvParameters)
{
    while (1)
    {
        switch (SIM7080_State)
        {
        case SIM7080_IDLE:
            if (xSemaphoreTake(SIM7080_ON_FLAG, portMAX_DELAY) == pdTRUE) // wait until another task requests ON.
            {
                xSemaphoreTake(SIM7080_OFF_FLAG, 0);
                Serial.println("SIM7080: STARTING");
                SIM7080_State = SIM7080_STARTING;
            }
            break;
        case SIM7080_STARTING:
            SIM7080_Driver_Init();
            Serial.println("SIM7080: CONNECTING");
            SIM7080_State = SIM7080_CONNECTING;

            break;

        case SIM7080_CONNECTING:
            if (xSemaphoreTake(SIM7080_OFF_FLAG, 0) == pdTRUE) // check whether shutdown is requested
            {
                SIM7080_State = SIM7080_OFF;
                break;
            }

            if (xSemaphoreTake(Sim7080ModemMutex, pdMS_TO_TICKS(5000) == pdTRUE))
            {
                bool ok = SIM7080_Driver_ConnectGPRS(SIM7080_APN);

                xSemaphoreGive(Sim7080ModemMutex);

                if (ok)
                {
                    Serial.println("SIM7080 GPRS connected successfully");
                    Serial.println("SIM7080: CONNECTED");
                    SIM7080_State = SIM7080_CONNECTED;
                    break;
                }
            }
            Serial.println("SIM7080 connection failed. Retrying in 5 seconds.");

            vTaskDelay(pdMS_TO_TICKS(5000));
            break;

        case SIM7080_CONNECTED:
            if (xSemaphoreTake(SIM7080_OFF_FLAG, pdMS_TO_TICKS(5000)) == pdTRUE)
            {
                SIM7080_State = SIM7080_OFF;
            }
            else
            {

                if (xSemaphoreTake(
                        Sim7080ModemMutex,
                        pdMS_TO_TICKS(5000)) == pdTRUE)
                {
                    bool linkOk =
                        SIM7080_Driver_IsConnected();

                    xSemaphoreGive(Sim7080ModemMutex);

                    if (linkOk)
                    {
                        xSemaphoreGive(SIM7080_CONNECTED_FLAG);
                    }
                    else
                    {
                        Serial.println(
                            "SIM7080 link lost. Reconnecting...");
                        Serial.println("SIM7080: CONNECTING");
                        SIM7080_State = SIM7080_CONNECTING;
                    }
                }
            }
            break;

        case SIM7080_OFF:
            Serial.println("SIM7080: OFF");

            if (xSemaphoreTake(
                    Sim7080ModemMutex,
                    portMAX_DELAY) == pdTRUE)
            {
                SIM7080_Driver_PowerOff();
                xSemaphoreGive(Sim7080ModemMutex);
            }

            // Discard an ON request that arrived during shutdown.
            xSemaphoreTake(SIM7080_ON_FLAG, 0);
            Serial.println("SIM7080: IDLE");
            SIM7080_State = SIM7080_IDLE;

            break;

        default:

            SIM7080_State = SIM7080_IDLE;

            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}