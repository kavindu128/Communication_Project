#include "LoRa_driver.h"

SPIClass LoRaSPI(HSPI);

bool LoRa_init(uint32_t frequency)
{
    Serial.println("Initializing LoRa SPI...");

    LoRaSPI.begin(
        LORA_SCK,
        LORA_MISO,
        LORA_MOSI,
        LORA_SS
    );

    LoRa.setSPI(LoRaSPI);

    LoRa.setPins(
        LORA_SS,
        LORA_RST,
        LORA_DIO0
    );

    if (!LoRa.begin(frequency))
    {
        Serial.println("LoRa.begin() FAILED");
        return false;
    }

    Serial.println("LoRa.begin() SUCCESS");

    return true;
}


bool LoRa_send(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0)
    {
        return false;
    }

    LoRa.beginPacket();

    LoRa.write(data, len);

    int result = LoRa.endPacket();

    return result == 1;
}

bool LoRa_receive(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0)
    {
        return false;
    }

    int packetSize = LoRa.parsePacket();

    if (packetSize == 0)
    {
        return false;
    }

    Serial.printf(
        "LoRa packet detected: %d bytes\n",
        packetSize
    );

    if (packetSize != len)
    {
        Serial.printf(
            "Packet size mismatch. Expected: %d, Received: %d\n",
            len,
            packetSize
        );

        while (LoRa.available())
        {
            LoRa.read();
        }

        return false;
    }

    uint16_t received = 0;

    while (LoRa.available() && received < len)
    {
        data[received] = LoRa.read();
        received++;
    }

    return received == len;
}
