#include "SIM7600_driver.h"
#include "SIM7600_hal.h"


// #define TINY_GSM_DEBUG Serial       
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
 
// Initialize the modem using the Serial2 interface provided by the HAL
static TinyGsm modem(SimSerial);
static TinyGsmClient cellularClient(modem);
static bool gprs_is_connected = false;
 
SemaphoreHandle_t ModemMutex = NULL;
 
void SIM7600_Driver_Init() {
    if (ModemMutex == NULL) {                  // create it only once
        ModemMutex = xSemaphoreCreateMutex();
    }
    SIM7600_HAL_Init();
 
    //Serial.println("Initializing TinyGSM modem...");
    // modem.restart() automatically sends "AT", "ATI", and checks basic responses
    if (xSemaphoreTake(ModemMutex, portMAX_DELAY) == pdTRUE) {
        modem.restart();
        xSemaphoreGive(ModemMutex);
    }
}
 
bool SIM7600_Driver_ConnectGPRS(const char* apn) {
    //Serial.printf("Connecting to APN: %s\n", apn);
    // gprsConnect() already opens the network (NETOPEN), nothing more to send
    gprs_is_connected = modem.gprsConnect(apn, "", "");
    return gprs_is_connected;
}
 
bool SIM7600_Driver_IsConnected() {
    gprs_is_connected = modem.isGprsConnected();
    return gprs_is_connected;
}
 
void SIM7600_Driver_PowerOff() {
    gprs_is_connected = false;
    cellularClient.stop();
    modem.gprsDisconnect();
    modem.poweroff();      // if this does not compile: modem.sendAT("+CPOF"); modem.waitResponse();
}
 
void SIM7600_Driver_SetDisconnected() {
    gprs_is_connected = false;
}
 
Client* SIM7600_Driver_GetClient() {
    return &cellularClient;
}
 
