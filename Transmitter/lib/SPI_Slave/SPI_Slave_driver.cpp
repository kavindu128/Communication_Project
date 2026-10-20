#include "SPI_Slave_driver.h"

DMA_ATTR static uint8_t dma_tx[SPI_FRAME_SIZE];
DMA_ATTR static uint8_t dma_rx[SPI_FRAME_SIZE];

bool SPI_Slave_Driver_Init(void)
{
    spi_bus_config_t bus_config = {};

    bus_config.mosi_io_num = SPI_MOSI;
    bus_config.miso_io_num = SPI_MISO;
    bus_config.sclk_io_num = SPI_SCK;
    bus_config.quadwp_io_num = -1;
    bus_config.quadhd_io_num = -1;
    bus_config.max_transfer_sz = SPI_FRAME_SIZE;

    spi_slave_interface_config_t slave_config = {};

    slave_config.spics_io_num = SPI_CS;
    slave_config.queue_size = 1;
    slave_config.mode = SPI_MODE0;

    esp_err_t result = spi_slave_initialize(
        SPI_HOST_USED,
        &bus_config,
        &slave_config,
        SPI_DMA_CH_AUTO
    );

    return result == ESP_OK;
}

bool SPI_Slave_Transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    if (tx_data == nullptr ||
        rx_data == nullptr ||
        len != SPI_FRAME_SIZE)
    {
        return false;
    }

    memcpy(dma_tx, tx_data, len);
    memset(dma_rx, 0, len);

    spi_slave_transaction_t transaction = {};

    transaction.length = len * 8;
    transaction.tx_buffer = dma_tx;
    transaction.rx_buffer = dma_rx;

    esp_err_t result = spi_slave_transmit(
        SPI_HOST_USED,
        &transaction,
        portMAX_DELAY
    );

    if (result != ESP_OK ||
        transaction.trans_len != len * 8)
    {
        return false;
    }

    memcpy(rx_data, dma_rx, len);

    return true;
}