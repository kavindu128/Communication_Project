#include "SPI_Master_driver.h"
#include "SPI_Master_app.h"
#include <stdint.h>
#include "Sensor_Packet.h"   


// Dummy packets
static sensor_packet_t sensorPackets[] =
{
    { .packet_id = 1, .water_ph = 7.2f, .turbidity = 15.5f, .soil_moisture = 62.0f, .soil_ph = 6.8f },
    { .packet_id = 2, .water_ph = 7.5f, .turbidity = 20.2f, .soil_moisture = 55.0f, .soil_ph = 6.5f },
    { .packet_id = 3, .water_ph = 6.9f, .turbidity = 12.8f, .soil_moisture = 70.0f, .soil_ph = 7.0f },
    { .packet_id = 4, .water_ph = 7.1f, .turbidity = 18.3f, .soil_moisture = 58.0f, .soil_ph = 6.7f },
    { .packet_id = 5, .water_ph = 7.4f, .turbidity = 10.5f, .soil_moisture = 75.0f, .soil_ph = 6.9f },
};

#define NUM_PACKETS (sizeof(sensorPackets) / sizeof(sensorPackets[0]))

void SPI_Master_app(void *pvParameters)
{
    // Initialize SPI
    if (!SPI_Master_Init())
    {
        Serial.println("SPI initialization failed");
        vTaskDelete(NULL);
        return; // vTaskDelete doesn't return, but keep this for clarity/safety
    }
 
    Serial.println("SPI initialized successfully");
 
    while (1)
    {
        for (size_t i = 0; i < NUM_PACKETS; i++)
        {
            // Fills in magic + crc automatically, sends the struct's
            // bytes unmodified, in order.
            bool sent = SPI_Master_Send_Packet(&sensorPackets[i]); 
            if (!sent)
            {
                Serial.println("SPI send failed");
            }
 
            Serial.printf(
                "Packet %u | Water pH: %.1f | Turbidity: %.1f | Soil Moisture: %.1f | Soil pH: %.1f\n",
                sensorPackets[i].packet_id,
                sensorPackets[i].water_ph,
                sensorPackets[i].turbidity,
                sensorPackets[i].soil_moisture,
                sensorPackets[i].soil_ph
            );
 
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}
