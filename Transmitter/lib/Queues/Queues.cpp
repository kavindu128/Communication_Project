#include "Queues.h"     
#include "SPI_Packet.h"

QueueHandle_t sensorData_from_SPI;   /* Board1 data → Transmeter task   */

QueueHandle_t networkStatus_to_SPI;       /* Network status → SPI task       */


void Queue_Init()
{
    sensorData_from_SPI = xQueueCreate(100, sizeof(SensorPacket));
    networkStatus_to_SPI = xQueueCreate(10, sizeof(NetworkStatusPacket));
    
    if (sensorData_from_SPI == NULL)
    {
        Serial.println("Failed to create sensor data queue");
    }
    else {
        Serial.println("Queues created successfully");
    }
}
