#include <Arduino.h>

#include "WIFI_app.h"
#include "LoRa_app.h"

TaskHandle_t WiFiAppHandle;
TaskHandle_t LoRaAppHandle;

void setup()
{
    // Start Serial Monitor
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("Receiver Starting...");
    Serial.println("==============================");


    // Create WiFi Receiver Task
    xTaskCreate(
        WIFI_app,           // Task function
        "WIFI App",    // Task name
        4096,               // Stack size
        NULL,               // Task parameter
        1,                  // Priority 
        &WiFiAppHandle             // Task handle
    );

     xTaskCreate(
        LoRa_app,
        "LoRa Task",
        4096,
        NULL,
        2,
        &LoRaAppHandle
    );
}


void loop()
{
    // Nothing required here
    vTaskDelay(pdMS_TO_TICKS(1000));
}