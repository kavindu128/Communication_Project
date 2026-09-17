#pragma once
#include <stdint.h>
 

#define SENSOR_PACKET_MAGIC 0xA55A
 
typedef struct __attribute__((packed)) {
    uint16_t magic;          // must equal SENSOR_PACKET_MAGIC
    uint8_t  packet_id;
    float    water_ph;
    float    turbidity;
    float    soil_moisture;
    float    soil_ph;
    uint8_t  crc;            // CRC8 over every byte above (not itself)
} sensor_packet_t;
