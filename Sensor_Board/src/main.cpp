#include <Arduino.h>
#include "SPI_app.h"

TaskHandle_t SPIAppHandle;

void setup()
{
    Serial.begin(115200);

    // Create SPI application task
    xTaskCreate(
        SPI_app,
        "SPI App",
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