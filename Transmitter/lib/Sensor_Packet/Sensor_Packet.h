#pragma once
// #include <stdint.h>
 
// Fixed sync word so the receiver can tell a real frame from garbage.
#define SENSOR_PACKET_MAGIC 0xA55A
 
// Packed, and its total size (20 bytes) is already a multiple of 4,
// which the ESP32 SPI-slave DMA engine requires. If you add fields,
// keep the total size a multiple of 4 or add explicit padding bytes.
typedef struct __attribute__((packed)) {
    uint16_t magic;          // must equal SENSOR_PACKET_MAGIC
    uint8_t  packet_id;
    float    water_ph;
    float    turbidity;
    float    soil_moisture;
    float    soil_ph;
    uint8_t  crc;            // CRC8 over every byte above (not itself)
} sensor_packet;
