#include "Queues.h"     
#include "Sensor_Packet.h"

QueueHandle_t sensorData_to_LoRa_Queue;   
QueueHandle_t ack_from_LoRa_Queue;        

QueueHandle_t sensorData_to_SIM7600_Queue; 
QueueHandle_t ack_from_SIM7600_Queue;     

QueueHandle_t sensorData_to_SIM7080_Queue; 
QueueHandle_t ack_from_SIM7080_Queue;      

QueueHandle_t sensorData_to_WiFi_Queue;  

#define QUEUE_LENGTH 100
#define QUEUE_ITEM_SIZE sizeof(sensor_packet)

void Queue_Init()
{
    sensorData_to_LoRa_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    
    sensorData_to_SIM7600_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    
    sensorData_to_SIM7080_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

    sensorData_to_WiFi_Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

   
    if (sensorData_to_LoRa_Queue == NULL ||
        sensorData_to_SIM7600_Queue == NULL ||
        sensorData_to_SIM7080_Queue == NULL ||
        sensorData_to_WiFi_Queue == NULL) {
        Serial.println("Failed to create one or more queues");
    }
    else {
        Serial.println("Queues created successfully");
    }
}
