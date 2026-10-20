#ifndef SPI_APP_H
#define SPI_APP_H

#include "SPI_Slave_driver.h"

extern TaskHandle_t SPIAppHandle;   

void SPI_Slave_app(void *pvParameters);
bool SPI_BuildFrame(SPIFrame *frame, uint8_t packet_type, const void *payload, uint8_t payload_length);
bool SPI_ValidateFrame(const SPIFrame *frame);

#endif