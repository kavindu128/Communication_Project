#ifndef LORA_DRIVER_H
#define LORA_DRIVER_H

#include "LoRa_hal.h"

#include <SPI.h>
#include <LoRa.h>

bool LoRa_init(uint32_t frequency);
bool LoRa_send(const uint8_t *data, uint16_t len);
bool LoRa_receive(uint8_t *data, uint16_t len);


#endif // LORA_DRIVER_H
