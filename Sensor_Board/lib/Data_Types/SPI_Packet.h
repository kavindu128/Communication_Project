#ifndef SPI_PACKET_H
#define SPI_PACKET_H

#include <Arduino.h>

#define SPI_FRAME_SIZE 68
#define SPI_HEADER_SIZE 2
#define SPI_PAYLOAD_SIZE 66

#define POLL_PACKET 0x00
#define SENSOR_PACKET 0x01
#define NETWORK_STATUS_PACKET 0x02A

typedef struct __attribute__((packed))
{
    uint32_t packet_id;
    float water_ph;
    float turbidity;
    float soil_moisture;
    float soil_ph;
} SensorPacket;

typedef struct __attribute__((packed))
{
    uint8_t connected;
    int8_t signal_strength;
    uint8_t connection_state;
} NetworkStatusPacket;

typedef struct __attribute__((packed))
{
    uint8_t data[SPI_FRAME_SIZE];
} SPIFrame;

#endif

