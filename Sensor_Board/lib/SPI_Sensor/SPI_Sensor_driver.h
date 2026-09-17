#ifndef SPI_SENSOR_DRIVER_H
#define SPI_SENSOR_DRIVER_H

#include "SPI_Sensor_hal.h"
#include "Sensor_Packet.h"
#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

// Function prototypes for SPI sensor interface
bool SPI_Sensor_Init(void);
bool SPI_Sensor_Send_Packet(sensor_packet_t *packet);
#endif // SPI_SENSOR_DRIVER_H