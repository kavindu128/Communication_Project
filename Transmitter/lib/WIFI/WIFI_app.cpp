#include "WIFI_driver.h"
#include "WIFI_app.h"
#include "WIFI_hal.h" 

TaskHandle_t WiFiAppHandle;



void WIFI_app(void *pvParameters) {
    WIFI_Driver_Init();
    
    
    WIFI_Driver_Connect(WIFI_SSID, WIFI_PASSWORD);
    
    bool wasConnected = false;

    for (;;) {
        if (WIFI_Driver_IsConnected()) {
            // Check if we just connected
            if (!wasConnected) {
                WIFI_Driver_PrintIP();
                wasConnected = true;
            }
        } else {
            // Check if we just lost connection
            if (wasConnected) {
                Serial.println("WiFi connection lost. Reconnecting...");
                wasConnected = false;
                
                // Restart the connection process
                WIFI_Driver_Init();
                WIFI_Driver_Connect(WIFI_SSID, WIFI_PASSWORD);
            }
        }
        
        // Yield the CPU back to other tasks
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}