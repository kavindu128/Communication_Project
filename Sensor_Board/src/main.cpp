#include <Arduino.h>
#include "SPI_Master_app.h"

TaskHandle_t SPIAppHandle;

QueueHandle_t sensorData_to_SPI;
QueueHandle_t networkStatus_from_SPI;

void setup()
{
    Serial.begin(115200);

    sensorData_to_SPI = xQueueCreate(5, sizeof(SensorPacket));
    networkStatus_from_SPI = xQueueCreate(5, sizeof(NetworkStatusPacket));

    if (sensorData_to_SPI == nullptr || networkStatus_from_SPI == nullptr)
    {
        Serial.println("Queue creation failed!");
        return;
    }

    // Create SPI application task
    xTaskCreate(
        SPI_Master_app,
        "SPI Master App",
        4096,
        NULL,
        1,
        &SPIAppHandle
    );
}

void loop()
{
    // Nothing here
}