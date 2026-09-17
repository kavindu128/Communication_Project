#include "SPI_Slave_driver.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include <string.h>
#include <stdio.h>

static uint8_t *rx_bufs[NUM_TRANS_BUFFERS];
static spi_slave_transaction_t trans[NUM_TRANS_BUFFERS];

static size_t dma_len(size_t len)
{
    // ESP32 SPI-slave DMA requires buffer length to be a multiple of 4.
    return (len + 3) & ~((size_t)3);
}

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

bool SPI_Slave_Init(void)
{
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = SPI_SLAVE_MOSI_PIN;
    buscfg.miso_io_num = SPI_SLAVE_MISO_PIN;
    buscfg.sclk_io_num = SPI_SLAVE_SCK_PIN;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    spi_slave_interface_config_t slvcfg = {};
    slvcfg.spics_io_num = SPI_SLAVE_CS_PIN;
    slvcfg.flags = 0;
    slvcfg.queue_size = NUM_TRANS_BUFFERS;
    slvcfg.mode = 0;

    // Keep idle lines from floating, which can look like extra clock
    // edges to the slave peripheral.
    gpio_set_pull_mode((gpio_num_t)SPI_SLAVE_MOSI_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)SPI_SLAVE_SCK_PIN,  GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)SPI_SLAVE_CS_PIN,   GPIO_PULLUP_ONLY);

    esp_err_t ret = spi_slave_initialize(SPI_HOST_USED, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        printf("SPI Slave initialization failed: %d\n", ret);
        return false;
    }

    size_t buf_len = dma_len(sizeof(sensor_packet));

    for (int i = 0; i < NUM_TRANS_BUFFERS; i++) {
        // DMA-capable, 4-byte-aligned buffer -- a stack/regular-heap
        // buffer here is the other classic cause of corrupted rx data.
        rx_bufs[i] = (uint8_t *)heap_caps_malloc(buf_len, MALLOC_CAP_DMA);
        if (!rx_bufs[i]) {
            printf("Failed to allocate DMA buffer %d\n", i);
            return false;
        }
        memset(rx_bufs[i], 0, buf_len);

        memset(&trans[i], 0, sizeof(spi_slave_transaction_t));
        trans[i].length    = buf_len * 8; // in bits
        trans[i].rx_buffer = rx_bufs[i];
        trans[i].tx_buffer = NULL;

        ret = spi_slave_queue_trans(SPI_HOST_USED, &trans[i], portMAX_DELAY);
        if (ret != ESP_OK) {
            printf("Failed to queue initial transaction %d\n", i);
            return false;
        }
    }

    printf("SPI Slave initialized successfully (queue depth=%d)\n", NUM_TRANS_BUFFERS);
    return true;
}

bool SPI_Slave_Receive(sensor_packet *packet)
{
    if (packet == NULL) {
        return false;
    }

    spi_slave_transaction_t *done_trans = NULL;
    esp_err_t ret = spi_slave_get_trans_result(SPI_HOST_USED, &done_trans, portMAX_DELAY);
    if (ret != ESP_OK || done_trans == NULL) {
        printf("SPI receive failed: %d\n", ret);
        return false;
    }

    size_t buf_len   = dma_len(sizeof(sensor_packet));
    size_t got_bytes = done_trans->trans_len / 8; // ACTUAL bits clocked in, not the max

    bool ok = false;

    if (got_bytes >= sizeof(sensor_packet)) {
        sensor_packet candidate;
        memcpy(&candidate, done_trans->rx_buffer, sizeof(sensor_packet));

        if (candidate.magic == SENSOR_PACKET_MAGIC) {
            uint8_t calc_crc = crc8((uint8_t *)&candidate, sizeof(sensor_packet) - 1);
            if (calc_crc == candidate.crc) {
                *packet = candidate;
                ok = true;
            } else {
                printf("SPI packet CRC mismatch (got 0x%02X, expected 0x%02X)\n",
                       candidate.crc, calc_crc);
            }
        } else {
            printf("SPI packet bad magic: 0x%04X\n", candidate.magic);
        }
    } else {
        printf("SPI short transaction: got %u bytes\n", (unsigned)got_bytes);
    }

    // Re-arm this same buffer immediately so the hardware is ready
    // again before the master's next CS pulse.
    memset(done_trans->rx_buffer, 0, buf_len);
    spi_slave_queue_trans(SPI_HOST_USED, done_trans, portMAX_DELAY);

    return ok;
}