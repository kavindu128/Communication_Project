#include "SIM7600_driver.h"
#include "SIM7600_hal.h"

// Must define the modem model before including TinyGSM
#define TINY_GSM_DEBUG Serial
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>

// Initialize the modem using the Serial2 interface provided by the HAL
static TinyGsm modem(SimSerial);
static TinyGsmClient cellularClient(modem);

// Cached link state. Every other task reads this instead of asking the modem.
static volatile bool gprs_is_connected = false;

// Protects ALL access to the modem UART (TinyGSM calls, PubSubClient over cellularClient)
SemaphoreHandle_t ModemMutex = NULL;

void SIM7600_Driver_Init() {
    // Create the mutex first so nobody can use the modem before it exists
    if (ModemMutex == NULL) {
        ModemMutex = xSemaphoreCreateMutex();
    }

    SIM7600_HAL_Init();

    Serial.println("Initializing TinyGSM modem...");

    xSemaphoreTake(ModemMutex, portMAX_DELAY);
    modem.restart();   // sends AT, ATI, resets and waits for the modem to be ready
    xSemaphoreGive(ModemMutex);
}

bool SIM7600_Driver_ConnectGPRS(const char* apn) {
    Serial.printf("Connecting to APN: %s\n", apn);

    bool ok = false;

    xSemaphoreTake(ModemMutex, portMAX_DELAY);
    // gprsConnect() already does CGDCONT, CGATT and NETOPEN internally,
    // so there is no need to send +NETOPEN manually.
    ok = modem.gprsConnect(apn, "", "");
    xSemaphoreGive(ModemMutex);

    gprs_is_connected = ok;
    return ok;
}

// Called ONLY from the SIM7600 task. Refreshes the cached state under the mutex.
void SIM7600_Driver_Poll() {
    xSemaphoreTake(ModemMutex, portMAX_DELAY);
    gprs_is_connected = modem.isGprsConnected();
    xSemaphoreGive(ModemMutex);
}

// Safe to call from any task: no AT traffic, just reads the cached flag.
bool SIM7600_Driver_IsConnected() {
    return gprs_is_connected;
}

Client* SIM7600_Driver_GetClient() {
    return &cellularClient;
}

void SIM7600_Driver_SetDisconnected() {
    gprs_is_connected = false;
}