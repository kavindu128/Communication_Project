#include <Arduino.h>
#include "SPI_Slave_app.h"
#include "Queues.h"
#include "WIFI_app.h"
#include "LoRa_app.h"

TaskHandle_t SPIAppHandle;
TaskHandle_t WiFiAppHandle;
TaskHandle_t LoRaAppHandle;


void setup()
{
    Serial.begin(115200);

    delay(1000);
    Serial.println("SPI Slave Starting...");

    Queue_Init();

    xTaskCreate(
        SPI_Slave_app,
        "SPI App",
        4096,
        NULL,
        3,
        &SPIAppHandle
    );

    // xTaskCreate(
    //     WIFI_app,             // Task function
    //     "WiFi App",           // Task name
    //     4096,                 // Stack size
    //     NULL,                 // Parameters
    //     1,                    // Priority
    //     &WiFiAppHandle        // Task handle
    // );

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
}

