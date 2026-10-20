#ifndef QUEUES_H
#define QUEUES_H

#include <Arduino.h>
#include "SPI_Packet.h"

extern QueueHandle_t sensorData_to_SPI;
extern QueueHandle_t networkStatus_from_SPI;

#endif