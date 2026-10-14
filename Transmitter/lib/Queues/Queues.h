#ifndef QUEUES_H
#define QUEUES_H

#include <Arduino.h>
#include "Sensor_Packet.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t sensorData_to_Transmeter;   /* Board1 data → Transmeter task   */


// Initialize queues
void Queue_Init();

#endif // QUEUES_H