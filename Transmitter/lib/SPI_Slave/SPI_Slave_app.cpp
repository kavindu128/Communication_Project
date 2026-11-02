
#include "SPI_Slave_app.h"

TaskHandle_t SPIAppHandle;

bool SPI_BuildFrame(
    SPIFrame *frame,
    uint8_t packet_type,
    const void *payload,
    uint8_t payload_length)
{
    if (frame == nullptr || payload_length > SPI_PAYLOAD_SIZE)
    {
        return false;
    }

    if (payload_length > 0 && payload == nullptr)
    {
        return false;
    }

    memset(frame->data, 0, SPI_FRAME_SIZE);

    frame->data[0] = packet_type;
    frame->data[1] = payload_length;

    if (payload_length > 0)
    {
        memcpy(
            &frame->data[SPI_HEADER_SIZE],
            payload,
            payload_length);
    }

    return true;
}

bool SPI_ValidateFrame(const SPIFrame *frame)
{
    if (frame == nullptr)
    {
        return false;
    }

    uint8_t packet_type = frame->data[0];
    uint8_t payload_length = frame->data[1];

    if (payload_length > SPI_PAYLOAD_SIZE)
    {
        return false;
    }

    switch (packet_type)
    {
    case POLL_PACKET:
        return payload_length == 0;

    case SENSOR_PACKET:
        return payload_length == sizeof(SensorPacket);

    case NETWORK_STATUS_PACKET:
        return payload_length == sizeof(NetworkStatusPacket);

    default:
        return false;
    }
}

void SPI_Slave_app(void *pvParameters)
{
    if (!SPI_Slave_Driver_Init())
    {
        Serial.println("SPI Slave initialization failed");
        vTaskDelete(NULL);
        return;
    }

    Serial.println("SPI Slave initialized successfully");

    SPIFrame txFrame = {};
    SPIFrame rxFrame = {};

    NetworkStatusPacket networkStatus = {};
    networkStatus.connected = 0;
    networkStatus.signal_strength = -127;
    networkStatus.connection_state = 0;

    // Duplicate-packet protection.
    uint32_t lastPacketId = 0;
    bool hasReceivedPacket = false;

    while (1)
    {
        // Update the latest network status from its queue.
        NetworkStatusPacket newNetworkStatus;

        if (xQueueReceive(
                networkStatus_to_SPI,
                &newNetworkStatus,
                0) == pdTRUE)
        {
            networkStatus = newNetworkStatus;
        }

        // Prepare the slave's outgoing network status.
        if (!SPI_BuildFrame(
                &txFrame,
                NETWORK_STATUS_PACKET,
                &networkStatus,
                sizeof(NetworkStatusPacket)))
        {
            Serial.println("Failed to build network status frame");
            vTaskDelay(1);
            continue;
        }

        // Exchange one fixed-size SPI frame.
        bool result = SPI_Slave_Transfer(
            txFrame.data,
            rxFrame.data,
            SPI_FRAME_SIZE);

        if (!result)
        {
            Serial.println("SPI transfer failed");
            continue;
        }

        if (!SPI_ValidateFrame(&rxFrame))
        {
            Serial.printf(
                "Invalid SPI frame: type=%u, length=%u\n",
                rxFrame.data[0],
                rxFrame.data[1]);
            continue;
        }

        uint8_t packetType = rxFrame.data[0];

        switch (packetType)
        {
        case POLL_PACKET:
        {
            // Poll is not sensor data.
            // Do not enqueue or publish sensor data.
            break;
        }

        case SENSOR_PACKET:
        {
            SensorPacket receivedPacket = {};

            memcpy(
                &receivedPacket,
                &rxFrame.data[SPI_HEADER_SIZE],
                sizeof(SensorPacket));

            // Ignore a repeated packet ID.
            if (hasReceivedPacket &&
                receivedPacket.packet_id == lastPacketId)
            {
                Serial.printf(
                    "Duplicate sensor packet ignored: ID=%lu\n",
                    (unsigned long)receivedPacket.packet_id);
                break;
            }

            lastPacketId = receivedPacket.packet_id;
            hasReceivedPacket = true;

            Serial.printf(
                "New sensor packet ID: %lu | "
                "Water pH: %.2f | "
                "Turbidity: %.2f | "
                "Soil Moisture: %.2f | "
                "Soil pH: %.2f\n",
                (unsigned long)receivedPacket.packet_id,
                receivedPacket.water_ph,
                receivedPacket.turbidity,
                receivedPacket.soil_moisture,
                receivedPacket.soil_ph);

            // Forward new sensor data to the Wi-Fi application.
            if (xQueueSend(
                    sensorData_from_SPI,
                    &receivedPacket,
                    0) != pdTRUE)
            {
                Serial.println("Sensor queue is full");
            }

            break;
        }

        default:
        {
            Serial.printf(
                "Unexpected packet type: 0x%02X\n",
                packetType);
            break;
        }
        }
    }
}
