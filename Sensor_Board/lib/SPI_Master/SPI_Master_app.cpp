#include "SPI_Master_app.h"   

static bool SPI_Master_BuildFrame(
    SPIFrame *frame,
    uint8_t packet_type,
    const void *payload,
    uint8_t payload_length
)
{
    if (frame == nullptr ||
        payload == nullptr ||
        payload_length > SPI_PAYLOAD_SIZE)
    {
        return false;
    }

    memset(frame->data, 0, SPI_FRAME_SIZE);

    frame->data[0] = packet_type;
    frame->data[1] = payload_length;

    memcpy(
        &frame->data[SPI_HEADER_SIZE],
        payload,
        payload_length
    );

    return true;
}

static bool SPI_Master_ValidateFrame(const SPIFrame *frame)
{
    if (frame == nullptr)
    {
        return false;
    }

    uint8_t type = frame->data[0];
    uint8_t length = frame->data[1];

    if (length > SPI_PAYLOAD_SIZE)
    {
        return false;
    }

    switch (type)
    {
        case SENSOR_PACKET:
            return length == sizeof(SensorPacket);

        case NETWORK_STATUS_PACKET:
            return length == sizeof(NetworkStatusPacket);

        default:
            return false;
    }
}


// Dummy packets
static SensorPacket sensorPackets[] =
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
    if (!SPI_Master_Driver_Init())
    {
        Serial.println("SPI initialization failed");
        vTaskDelete(NULL);
        return; // vTaskDelete doesn't return, but keep this for clarity/safety
    }
 
    Serial.println("SPI initialized successfully");
    
    SPIFrame txFrame = {};
    SPIFrame rxFrame = {};

    // Keep the latest sensor values.
    SensorPacket sensor ;
    NetworkStatusPacket networkStatus;

    size_t index = 0;
    size_t packetCount = sizeof(sensorPackets) / sizeof(sensorPackets[0]);

     while (1)
    {
        while (1)
    {
        // 1. Generate/select dummy sensor data
        sensor = sensorPackets[index];

        // 2. Put generated sensor data into Queue 1
        if (xQueueSend(sensorData_to_SPI, &sensor, 0) == pdTRUE)
        {
            Serial.printf(
                "\nGenerated sensor packet ID: %u\n",
                sensor.packet_id);
        }
        else
        {
            Serial.println("Sensor queue is full");
        }

        // 3. Read sensor data from Queue 1 for SPI transmission
        if (xQueueReceive(sensorData_to_SPI, &sensor, 0) == pdTRUE)
        {
            // Build the outgoing sensor frame
            if (SPI_Master_BuildFrame(
                    &txFrame,
                    SENSOR_PACKET,
                    &sensor,
                    sizeof(SensorPacket)))
            {
                // 4. Send sensor frame and simultaneously receive
                // the frame returned by the SPI slave
                if (SPI_Master_Transfer(
                        txFrame.data,
                        rxFrame.data,
                        SPI_FRAME_SIZE))
                {
                    Serial.printf(
                        "Sensor packet %u sent through SPI\n",
                        sensor.packet_id);

                    // 5. Validate the frame received from the slave
                    if (SPI_Master_ValidateFrame(&rxFrame) &&
                        rxFrame.data[0] == NETWORK_STATUS_PACKET &&
                        rxFrame.data[1] == sizeof(NetworkStatusPacket))
                    {
                        // Decode the network status payload
                        memcpy(
                            &networkStatus,
                            &rxFrame.data[SPI_HEADER_SIZE],
                            sizeof(NetworkStatusPacket));

                        // 6. Put received status into Queue 2
                        if (xQueueSend(
                                networkStatus_from_SPI,
                                &networkStatus,
                                0) != pdTRUE)
                        {
                            Serial.println(
                                "Network status queue is full");
                        }
                    }
                    else
                    {
                        Serial.println(
                            "Invalid network status received from slave");
                    }
                }
                else
                {
                    Serial.println("SPI transfer failed");
                }
            }
        }

        // 7. Read ONLY network status from Queue 2 and print it
        while (xQueueReceive(
                   networkStatus_from_SPI,
                   &networkStatus,
                   0) == pdTRUE)
        {
            Serial.println("\n--- Network Status from SPI Slave ---");

            Serial.printf(
                "Connected: %s\n",
                networkStatus.connected ? "YES" : "NO");

            Serial.printf(
                "Signal strength: %d dBm\n",
                networkStatus.signal_strength);

            Serial.printf(
                "Connection state: %u\n",
                networkStatus.connection_state);

            Serial.println("-------------------------------------");
        }

        // 8. Select the next dummy sensor packet
        index = (index + 1) % packetCount;

        // Repeat every second
        vTaskDelay(pdMS_TO_TICKS(60000));
    }

    }
}
