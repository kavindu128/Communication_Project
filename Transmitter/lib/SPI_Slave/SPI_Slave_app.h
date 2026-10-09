#ifndef SPI_APP_H
#define SPI_APP_H

#include "SPI_Slave_driver.h"
extern TaskHandle_t SPIAppHandle;   

void SPI_Slave_app(void *pvParameters);

#endif