#include "Queues.h"     
#include "Sensor_Packet.h"

QueueHandle_t sensorData_to_Transmeter;   /* Board1 data → Transmeter task   */
 

#define QUEUE_LENGTH 100
#define QUEUE_ITEM_SIZE sizeof(sensor_packet)

void Queue_Init()
{
    sensorData_to_Transmeter = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    

   
    if (sensorData_to_Transmeter == NULL)
    {
        Serial.println("Failed to create sensor data queue");
    }
    else {
        Serial.println("Queues created successfully");
    }
}
