#include "SPI_Slave_app.h"
#include "SPI_Packet.h"
#include "Queues.h"

TaskHandle_t SPIAppHandle;

bool SPI_BuildFrame(SPIFrame *frame, uint8_t packet_type, const void *payload, uint8_t payload_length)
{
    if (frame == nullptr || payload == nullptr)
    {
        return false;
    }

    if (payload_length > SPI_PAYLOAD_SIZE)
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

    // Check that the packet type is recognized.
    switch (packet_type)
    {
        case SENSOR_PACKET:
        case NETWORK_STATUS_PACKET:
        //case PACKET_CONTROL:
        //case PACKET_DIAGNOSTIC:
            return true;

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
    }
 
    Serial.println("SPI Slave initialized successfully");
 
    SPIFrame txFrame = {};
    SPIFrame rxFrame = {};

    // Default status until an actual status is received from the queue.
    NetworkStatusPacket NetworkStatus = {};
    NetworkStatus.connected = 0;
    NetworkStatus.signal_strength = -127;
    NetworkStatus.connection_state = 0;

 
    while (1)
    {
        NetworkStatusPacket newNetworkStatus;

        if (xQueueReceive(networkStatus_to_SPI, &newNetworkStatus, 0) == pdTRUE)
        {
            NetworkStatus = newNetworkStatus;
        }

        // Prepare status before the master starts the transaction.
        if (!SPI_BuildFrame(&txFrame, NETWORK_STATUS_PACKET, &NetworkStatus,sizeof(NetworkStatus)))
        {
            Serial.println("Failed to build Networkstatus frame");
            continue;
        }

        bool result = SPI_Slave_Transfer(txFrame.data, rxFrame.data, SPI_FRAME_SIZE);
 
        if (!result)
        {
            Serial.println("SPI transfer failed");
            continue;
        }

        if (!SPI_ValidateFrame(&rxFrame))
        {
            Serial.println("Received invalid frame");
            continue;
        }

        switch (rxFrame.data[0])
        {
            case SENSOR_PACKET:
            {
                if (rxFrame.data[1] != sizeof(SensorPacket))
                {
                    Serial.println("Invalid sensor payload length");
                    break;
                }

                SensorPacket receivedPacket;

                memcpy(
                    &receivedPacket,
                    &rxFrame.data[SPI_HEADER_SIZE],
                    sizeof(receivedPacket)
                );

                Serial.printf(
                    "Sensor data received | "
                    "Water pH: %.2f | "
                    "Turbidity: %.2f | "
                    "Soil Moisture: %.2f | "
                    "Soil pH: %.2f\n",
                    receivedPacket.water_ph,
                    receivedPacket.turbidity,
                    receivedPacket.soil_moisture,
                    receivedPacket.soil_ph
                );

                if (xQueueSend(sensorData_from_SPI, &receivedPacket,0) != pdTRUE)
                {
                    Serial.println("Sensor queue is full");
                }

                break;
            }

            default:
                Serial.printf(
                    "Unexpected packet type from master: 0x%02X\n",
                    rxFrame.data[0]
                );
                break;
        }
        
    }
}
