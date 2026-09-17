#include <Arduino.h>

#include "WIFI_app.h"
#include "Sensor_Packet.h"
#include "Queues.h"

void WIFI_app(void *pvParameters)
{
    Serial.println("WiFi App Starting...");

    if (!WIFI_Init())
    {
        Serial.println("WiFi initialization failed");
        vTaskDelete(NULL);
    }

    Serial.println("WiFi initialized successfully");

    static sensor_packet Packet;

    while (1)
    {
        if (xQueueReceive(sensorData_to_WiFi_Queue, &Packet, portMAX_DELAY))
        {
            Serial.printf(
                "Packet Received from Queue: Packet %d | "
                "Water pH: %.1f | "
                "Turbidity: %.1f | "
                "Soil Moisture: %.1f | "
                "Soil pH: %.1f\n",

                Packet.packet_id,
                Packet.water_ph,
                Packet.turbidity,
                Packet.soil_moisture,
                Packet.soil_ph
            );
        }

        // Send packet through WiFi
        if (WIFI_SendData((const uint8_t *)&Packet, sizeof(sensor_packet)))
        {  
            Serial.printf(
                "WiFi: Packet %d sent successfully\n",
                Packet.packet_id
            );
        }
        else
        {
            Serial.printf(
                "WiFi: Packet %d send failed\n",
                Packet.packet_id
            );
        }
    }

}

