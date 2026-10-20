#ifndef SPI_MASTER_DRIVER_H
#define SPI_MASTER_DRIVER_H

#include <Arduino.h>
#include <SPI.h>

#include "SPI_Master_hal.h"
#include "SPI_Packet.h"
#include "Queues.h"

bool SPI_Master_Driver_Init(void);
bool SPI_Master_Transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len);

#endif // SPI_MASTER_DRIVER_H