#ifndef QUEUES_H
#define QUEUES_H

#include <Arduino.h>
#include "SPI_Packet.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t sensorData_from_SPI;  /* Board1 data → Transmeter task   */
extern QueueHandle_t networkStatus_to_SPI; /* Network status → SPI task       */

// Initialize queues
void Queue_Init();

#endif // QUEUES_H