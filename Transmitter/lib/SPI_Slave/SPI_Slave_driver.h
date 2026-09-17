# ifndef SPI_SLAVE_DRIVER_H
# define SPI_SLAVE_DRIVER_H

#include "SPI_Slave_hal.h"
#include <Arduino.h>
#include <stdint.h>
#include "Sensor_Packet.h"

bool SPI_Slave_Init(void);

bool SPI_Slave_Receive(sensor_packet_t *packet);

#endif /* SPI_SLAVE_DRIVER_H */