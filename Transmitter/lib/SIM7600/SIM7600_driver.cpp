#include "SIM7600_driver.h"
#include "SIM7600_hal.h"

// Initialize the modem using the Serial2 interface provided by the HAL
static TinyGsm modem(SimSerial);
static TinyGsmClient cellularClient(modem);
static volatile bool gprs_is_connected = false; // cached: read by other tasks without AT commands

SemaphoreHandle_t Sim7600ModemMutex = NULL;

void SIM7600_Driver_Init()
{
    gprs_is_connected = false; // modem is (re)starting: no link yet
    SIM7600_HAL_Init();

    // Serial.println("Initializing TinyGSM modem...");
    //  modem.restart() automatically sends "AT", "ATI", and checks basic responses
    if (xSemaphoreTake(Sim7600ModemMutex, portMAX_DELAY) == pdTRUE)
    {
        modem.restart();
        xSemaphoreGive(Sim7600ModemMutex);
    }
}

bool SIM7600_Driver_ConnectGPRS(const char *apn)
{
    // Serial.printf("Connecting to APN: %s\n", apn);
    //  gprsConnect() already opens the network (NETOPEN), nothing more to send
    gprs_is_connected = modem.gprsConnect(apn, "", "");
    return gprs_is_connected;
}

bool SIM7600_Driver_IsConnected()
{
    gprs_is_connected = modem.isGprsConnected();
    return gprs_is_connected;
}

// Cached flag only - NO AT command, safe to call from any task at any time
bool SIM7600_Driver_IsLinkUp()
{
    return gprs_is_connected;
}

void SIM7600_Driver_PowerOff()
{
    gprs_is_connected = false;
    cellularClient.stop();
    modem.gprsDisconnect();
    modem.poweroff(); // if this does not compile: modem.sendAT("+CPOF"); modem.waitResponse();
}

void SIM7600_Driver_SetDisconnected()
{
    gprs_is_connected = false;
}

Client *SIM7600_Driver_GetClient()
{
    return &cellularClient;
}

int SIM7600_Driver_GetSignalStrength()
{
    // Serial.println("SIM7600: Waiting for modem mutex...");

    if (xSemaphoreTake(Sim7600ModemMutex, pdMS_TO_TICKS(3000)) != pdTRUE)
    {
        //Serial.println("SIM7600: Mutex BUSY!");
        return -2;
    }

    // Serial.println("SIM7600: Mutex acquired");

    int signal = -1;

    if (modem.testAT())
    {
        signal = modem.getSignalQuality();
    }

    xSemaphoreGive(Sim7600ModemMutex);

    // Serial.println("SIM7600: Mutex released");

    return signal;
}