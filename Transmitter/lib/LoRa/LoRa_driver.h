#ifndef LORA_DRIVER_H
#define LORA_DRIVER_H

#include "LoRa_hal.h"
#include <string.h>
#include <stdio.h>

bool LoRa_init(uint32_t frequency);
bool LoRa_sendPacket(const uint8_t *data, uint16_t len);


#endif // LORA_DRIVER_H
