#ifndef QUEUES_H
#define QUEUES_H

#include <Arduino.h>
#include "Sensor_Packet.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t sensorData_to_LoRa_Queue;   /* Board1 data → LoRa task   */
extern QueueHandle_t ack_from_LoRa_Queue;         /* LoRa ACK  → SPI slave     */


extern QueueHandle_t sensorData_to_SIM7600_Queue; /* Board1 data → SIM7600 task */
extern QueueHandle_t ack_from_SIM7600_Queue;      /* SIM7600 ACK → SPI slave    */


extern QueueHandle_t sensorData_to_SIM7080_Queue; /* Board1 data → SIM7080 task */
extern QueueHandle_t ack_from_SIM7080_Queue;      /* SIM7080 ACK → SPI slave    */


extern QueueHandle_t sensorData_to_WiFi_Queue;    /* Board1 data → WiFi task    */
extern QueueHandle_t ack_from_WiFi_Queue;      


// Initialize queues
void Queue_Init();

#endif // QUEUES_H