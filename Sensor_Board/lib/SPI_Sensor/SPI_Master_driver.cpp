#include "SPI_Master_driver.h"
#include <SPI.h>
#include "Sensor_Packet.h"
#include <string>
#include<stdint.h>


static uint8_t crc8(const uint8_t *data, size_t len)
{
    uint8_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}
 
bool SPI_Master_Init(void)
{
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
    pinMode(SPI_CS, OUTPUT);
    digitalWrite(SPI_CS, HIGH);
    return true;
}

bool SPI_Master_Send_Packet(sensor_packet_t *packet)
{
    if (packet == NULL) {
        return false;
    }
 
    packet->magic = SENSOR_PACKET_MAGIC;
    packet->crc   = crc8((uint8_t *)packet, sizeof(sensor_packet_t) - 1);
 
    // Keep this well below what the ESP32 slave can reliably clock in
    // (1 MHz is a safe starting point on GPIO-matrix pins).
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
 
    digitalWrite(SPI_CS, LOW);
    // Small settle time before the first clock edge so the slave's
    // hardware is fully ready to shift bits in.
    delayMicroseconds(20);
 
    const uint8_t *bytes = (const uint8_t *)packet;
    for (size_t i = 0; i < sizeof(sensor_packet_t); i++) {
        SPI.transfer(bytes[i]);   // sends each byte unmodified, in order
    }
 
    digitalWrite(SPI_CS, HIGH);
    SPI.endTransaction();
 
    delay(2);
 
    return true;
}




