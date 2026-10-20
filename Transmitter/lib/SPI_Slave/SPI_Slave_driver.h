# ifndef SPI_SLAVE_DRIVER_H
# define SPI_SLAVE_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include "driver/spi_slave.h"

#include "SPI_Slave_hal.h"
#include "SPI_Packet.h"

bool SPI_Slave_Driver_Init(void);

bool SPI_Slave_Transfer( const uint8_t *tx_data, uint8_t *rx_data, size_t len);

#endif /* SPI_SLAVE_DRIVER_H */