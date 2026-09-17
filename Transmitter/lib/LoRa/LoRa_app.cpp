#include "LoRa_app.h"
#include "Sensor_Packet.h"

void LoRa_app(void* pvParameters)
{
    Serial.println("LoRa Transmitter Starting...");

    if (!LoRa_init(433E6))
    {
        Serial.println("LoRa initialization failed");
        vTaskDelete(NULL);
        return;
    }

    Serial.println("LoRa initialized successfully");

     static sensor_packet_t packet;

    while (1)
    {
        // Wait for SensorPacket from queue
        if (xQueueReceive(
                sensorData_to_LoRa_Queue,
                &packet,
                portMAX_DELAY))
        {
            Serial.printf(
                "Packet Received from Queue to LoRa: Packet %d | "
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

            // Send SensorPacket through LoRa
            if (LoRa_send(
                    (const uint8_t *)&packet,
                    sizeof(sensor_packet_t)))
            {
                Serial.printf(
                    "LoRa: Packet %d sent successfully\n",
                    packet.packet_id
                );
            }
            else
            {
                Serial.printf(
                    "LoRa: Packet %d send failed\n",
                    packet.packet_id
                );
            }
        }
    }
}

