#include <Arduino.h>
#include "SPI_Slave_app.h"
#include "Queues.h"
#include "WIFI_app.h"
#include "LoRa_app.h"
#include "SIM7600_app.h"
#include "SIM7080_app.h"
#include "WIFI_app.h"
#include "MQTT_app.h"
#include "NetworkManager_app.h"

void setup()
{
    Serial.begin(115200);

    vTaskDelay(pdMS_TO_TICKS(1000));

    Queue_Init();
    SIM7600_app_Init();
    WIFI_app_init();
    SIM7080_app_Init();
    LoRa_app_Init();

    xTaskCreatePinnedToCore(NetworkManager_app, "NetworkManager_app", 4096, NULL, 1, &NetworkManagerAppHandle, 1);

    xTaskCreatePinnedToCore(SIM7080_app, "SIM7080_app", 4096, NULL, 1, &SIM7080AppHandle, 1);

    xTaskCreatePinnedToCore(SIM7600_app, "SIM7600_app", 4096, NULL, 1, &SIM7600AppHandle, 1);

    xTaskCreatePinnedToCore(WIFI_app, "WiFi App", 4096, NULL, 1, &WiFiAppHandle, 1);

    xTaskCreatePinnedToCore(MQTT_app, "MQTT App", 4096, NULL, 1, &MQTTAppHandle, 1);

    xTaskCreatePinnedToCore(SPI_Slave_app, "SPI App", 4096, NULL, 3, &SPIAppHandle, 1); // xTaskCreatePinnedToCore

    xTaskCreatePinnedToCore(LoRa_app, "LoRa Task", 4096, NULL, 2, &LoRaAppHandle, 1);
   
}

void loop()
{
}
