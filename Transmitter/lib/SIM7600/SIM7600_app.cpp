#include "SIM7600_app.h"                   

TaskHandle_t SIM7600AppHandle;

// Set to your local APN (e.g., "dialogbb" or "mobitel")
const char* APN = "mobitel"; 

void SIM7600_app(void *pvParameters) {
    Serial.println("Starting SIM7600G Task...");
    
    // Boot the hardware and initialize TinyGSM
    SIM7600_Driver_Init();
    
    bool wasConnected = false;

    for (;;) {
    SIM7600_Driver_Poll();   // refresh the cached state, once per loop

    if (SIM7600_Driver_IsConnected()) {
        if (!wasConnected) {
            Serial.println("SIM7600 Cellular Network Connected!");
            wasConnected = true;
        }
    } else {
        if (wasConnected) {
            Serial.println("Cellular connection lost. Reconnecting...");
            wasConnected = false;
        }

        if (SIM7600_Driver_ConnectGPRS(APN)) {
            Serial.println("GPRS Connection successful.");
        } else {
            Serial.println("GPRS Connection failed. Retrying in 5 seconds...");
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    vTaskDelay(pdMS_TO_TICKS(5000));   // poll every 5 s instead of 1 s
}
}