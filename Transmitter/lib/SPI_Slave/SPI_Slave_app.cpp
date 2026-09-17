#include "SPI_Slave_driver.h"
#include "SPI_Slave_app.h"
#include "Sensor_Packet.h"
#include "Queues.h"




void SPI_Slave_app(void *pvParameters)
{
    if (!SPI_Slave_Init())
    {
        Serial.println("SPI Slave initialization failed");
        vTaskDelete(NULL);
    }
 
    Serial.println("SPI Slave initialized successfully");
 
    // Plain struct is fine here -- SPI_Slave_Receive() copies the
    // validated packet into it; the actual DMA-capable buffers are
    // allocated internally by SPI_Slave_Init().
    static sensor_packet_t receivedPacket;
 
    while (1)
    {
        // Blocks for one transaction, checks magic + CRC internally,
        // and only returns true if receivedPacket now holds a valid frame.
        bool result = SPI_Slave_Receive(&receivedPacket);
 
        if (result)
        {
            Serial.printf(
                "Packet Received to Queues: Packet %d | "
                "Water pH: %.1f | "
                "Turbidity: %.1f | "
                "Soil Moisture: %.1f | "
                "Soil pH: %.1f\n",
 
                receivedPacket.packet_id,
                receivedPacket.water_ph,
                receivedPacket.turbidity,
                receivedPacket.soil_moisture,
                receivedPacket.soil_ph
            );
 
            xQueueSend(sensorData_to_LoRa_Queue, &receivedPacket, 0);
            xQueueSend(sensorData_to_SIM7600_Queue, &receivedPacket, 0);
            xQueueSend(sensorData_to_SIM7080_Queue, &receivedPacket, 0);
            xQueueSend(sensorData_to_WiFi_Queue, &receivedPacket, 0);
        }
        else
        {
            // Includes both a hardware receive failure and a rejected
            // frame (bad magic / CRC mismatch) -- either way, nothing
            // bad gets pushed into the queues.
            Serial.println("SPI receive failed or packet invalid");
        }
    }
}
