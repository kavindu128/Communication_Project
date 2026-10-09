# ifndef SPI_SLAVE_DRIVER_H
# define SPI_SLAVE_DRIVER_H

#include <string.h>
#include <stdio.h>
#include "SPI_Slave_hal.h"
#include "Sensor_Packet.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/spi_slave.h"

bool SPI_Slave_Init(void);

bool SPI_Slave_Receive(sensor_packet *packet);

#endif /* SPI_SLAVE_DRIVER_H */