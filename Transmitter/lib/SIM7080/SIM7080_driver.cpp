# include "SIM7080_driver.h"

#define TINY_GSM_MODEM_SIM7080
#include <TinyGsmClient.h>

static TinyGsm modem(Sim7080Serial);
static TinyGsmClient cellularClient(modem);

static volatile bool gprs_is_connected = false;

SemaphoreHandle_t Sim7080ModemMutex = NULL;

void SIM7080_Driver_Init()
{
    if (Sim7080ModemMutex == NULL)
    {
        Sim7080ModemMutex = xSemaphoreCreateMutex();
    }

    if (Sim7080ModemMutex == NULL)
    {
        Serial.println("Failed to create SIM7080 mutex");
        return;
    }

    SIM7080_HAL_Init();

    if (xSemaphoreTake(Sim7080ModemMutex, portMAX_DELAY) == pdTRUE)
    {
        // If the module is already ON, do not pulse PWRKEY.
        if (!modem.testAT(1000))
        {
            Serial.println("SIM7080 not responding. Trying power ON...");
            powerOnSIM7080G();
        }

        // Wait for the module to become responsive.
        bool modemReady = false;

        for (int i = 0; i < 10; i++)
        {
            if (modem.testAT(1000))
            {
                modemReady = true;
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        if (modemReady)
        {
            modem.init();
            Serial.println("SIM7080 is responding");
        }
        else
        {
            Serial.println("SIM7080 failed to respond");
        }

        xSemaphoreGive(Sim7080ModemMutex);
    }
}

bool SIM7080_Driver_ConnectGPRS(const char* apn)
{
    gprs_is_connected = modem.gprsConnect(apn, "","");

    return gprs_is_connected;
}

bool SIM7080_Driver_IsConnected()
{
    gprs_is_connected = modem.isGprsConnected();

    return gprs_is_connected;
}

bool SIM7080_Driver_IsLinkUp()
{
    return gprs_is_connected;
}


void SIM7080_Driver_PowerOff()
{
    gprs_is_connected = false;
    cellularClient.stop();
    modem.gprsDisconnect();
    modem.poweroff();
    //powerOnSIM7080G();
}

void SIM7080_Driver_SetDisconnected()
{
    gprs_is_connected = false;
}

Client* SIM7080_Driver_GetClient()
{
    return &cellularClient;
}


