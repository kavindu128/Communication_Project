#ifndef SPI_MASTER_DRIVER_H
#define SPI_MASTER_DRIVER_H

#include "SPI_Master_hal.h"
#include "Sensor_packet.h"
#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>

// Function prototypes for SPI sensor interface
bool SPI_Master_Init(void);
bool SPI_Master_Send_Packet(sensor_packet_t *packet);
#endif // SPI_MASTER_DRIVER_H