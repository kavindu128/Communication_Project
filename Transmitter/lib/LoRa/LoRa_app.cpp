#include "LoRa_app.h"
#include "SPI_Packet.h"
#include "NetworkManager_app.h"

TaskHandle_t LoRaAppHandle;
volatile bool LoRa_IsReady = false;

void LoRa_app(void* pvParameters)
{
     Serial.println("LoRa Transmitter Starting...");

    if (!LoRa_init(433E6))
    {
        LoRa_IsReady = false;
        Serial.println("LoRa initialization failed");
        vTaskDelete(NULL);
        return;
    }

    LoRa_IsReady = true;
    Serial.println("LoRa initialized successfully");

     static SensorPacket packet;

    while (1)
    {
        // Do not consume the shared sensor queue unless LoRa is the selected route.
        if (!NetworkManager_IsRouteSelected(NETWORK_LORA))
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        // Short wait lets the task notice a route change promptly.
        if (xQueueReceive(
                sensorData_from_SPI,
                &packet,
                pdMS_TO_TICKS(100)))
        { 
            if (!NetworkManager_IsRouteSelected(NETWORK_LORA))
            {
                // Route changed while waiting; keep the packet for the selected sender.
                xQueueSendToFront(sensorData_from_SPI, &packet, 0);
                continue;
            }
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
                    sizeof(SensorPacket)))
            {
                Serial.printf(
                    "LoRa: Packet %d sent successfully\n",
                    packet.packet_id
                );
            }
            else
            {
                // Keep the packet for the next active route/retry.
                xQueueSendToFront(sensorData_from_SPI, &packet, 0);
                Serial.printf(
                    "LoRa: Packet %d send failed; packet retained\n",
                    packet.packet_id
                );
            }
        }
        
    }

}

