#include "SPI_Master_driver.h"
#include <SPI.h>

#include "SPI_Packet.h"

static SPIClass spi(FSPI);
 
bool SPI_Master_Driver_Init(void)
{
    pinMode(SPI_CS, OUTPUT);
    digitalWrite(SPI_CS, HIGH);

    spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);

    return true;
}


bool SPI_Master_Transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    if (tx_data == nullptr || rx_data == nullptr || len != SPI_FRAME_SIZE)
    {
        return false;
    }

    spi.beginTransaction(
        SPISettings(SPI_CLOCK_HZ, MSBFIRST, SPI_MODE0)
    );

    digitalWrite(SPI_CS, LOW);

    spi.transferBytes(const_cast<uint8_t *>(tx_data), rx_data, len);

    digitalWrite(SPI_CS, HIGH);

    spi.endTransaction();

    return true;
}




