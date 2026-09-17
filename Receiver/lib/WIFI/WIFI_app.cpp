#include "WIFI_app.h"
#include "WIFI_driver.h"
#include "Sensor_Packet.h"

void WIFI_app(void *pvParameters)
{
    if (!WIFI_Init())
    {
        Serial.println("WiFi initialization failed");

        vTaskDelete(NULL);
    }

    Serial.println("WiFi initialized successfully");

     static sensor_packet_t packet;

    while (1)
    {
        if (WIFI_ReceiveData(
                (uint8_t *)&packet,
                sizeof(sensor_packet_t)))
        {
            Serial.printf(
                "Packet Received: Packet %d | "
                "Water pH: %.1f | "
                "Turbidity: %.1f | "
                "Soil Moisture: %.1f | "
                "Soil pH: %.1f\n",

                packet.packet_id,
                packet.water_ph,
                packet.turbidity,
                packet.soil_moisture,
                packet.soil_ph
            );
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}