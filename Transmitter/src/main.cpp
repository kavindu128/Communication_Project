#include <Arduino.h>
#include "SPI_Slave_app.h"
#include "Queues.h"
#include "WIFI_app.h"
#include "LoRa_app.h"
#include "SIM7600_app.h"
#include "MQTT_app.h"
#include "sim7600test_app.h"



void setup()
{
    Serial.begin(115200);

    vTaskDelay(pdMS_TO_TICKS(1000));

    Queue_Init();
    SIM7600_app_init();

    xTaskCreatePinnedToCore(SIM7600_Test_app, "SIM7600_Test_app", 4096, NULL, 1, &SIM7600TestHandle,1);
    

    xTaskCreatePinnedToCore(SIM7600_app, "SIM7600_app", 4096, NULL, 1, &SIM7600AppHandle, 1);

    //xTaskCreatePinnedToCore(WIFI_app, "WiFi App", 4096, NULL, 1, &WiFiAppHandle,1);

    xTaskCreatePinnedToCore(MQTT_app, "MQTT App", 4096, NULL, 1, &MQTTAppHandle,1);

    xTaskCreatePinnedToCore(SPI_Slave_app, "SPI App", 4096, NULL, 3, &SPIAppHandle,1); // xTaskCreatePinnedToCore

    

    //xTaskCreatePinnedToCore(LoRa_app, "LoRa Task", 4096, NULL, 2, &LoRaAppHandle,1);

    
}
    

void loop()
{
}
