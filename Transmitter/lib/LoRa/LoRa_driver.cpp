#include "LoRa_driver.h"
#include <string.h>

HardwareSerial LoRaSerial(0);

static bool LoRa_SendCommand(const char *command)
{
    if (command == nullptr)
    {
        return false;
    }

    char response[256] = {0};
    uint16_t index = 0;

    // Clear old responses
    while (LoRaSerial.available())
    {
        LoRaSerial.read();
    }

    Serial.print("\nLoRa AT Command: ");
    Serial.println(command);

    // Send command
    LoRaSerial.print(command);
    LoRaSerial.print("\r\n");

    uint32_t startTime = millis();

    while (millis() - startTime < 3000)
    {
        while (LoRaSerial.available())
        {
            char c = LoRaSerial.read();

            Serial.write(c);

            if (index < sizeof(response) - 1)
            {
                response[index++] = c;
                response[index] = '\0';
            }

            if (strstr(response, "ERROR") != nullptr ||
                strstr(response, "AT_PARAM_ERROR") != nullptr ||
                strstr(response, "AT_BUSY_ERROR") != nullptr)
            {
                Serial.println("\nLoRa command returned an error!");
                return false;
            }

            if (strstr(response, "OK") != nullptr)
            {
                Serial.println("\nLoRa command accepted!");
                return true;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    Serial.println("\nLoRa command timeout!");
    Serial.print("Received response: ");
    Serial.println(response);

    return false;
}

bool LoRa_init(uint32_t frequency)
{

    Serial.println("\nLoRa UART initialization...");
    pinMode(LORA_RST, OUTPUT);
    digitalWrite(LORA_RST, LOW);

    vTaskDelay(pdMS_TO_TICKS(100));

    digitalWrite(LORA_RST, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1500));

    LoRaSerial.begin(LORA_BAUDRATE, SERIAL_8N1, LORA_RX, LORA_TX);

    vTaskDelay(pdMS_TO_TICKS(2000));

    if (!LoRa_SendCommand("AT"))
    {
        // Serial.println("ERROR: RAK3172 AT communication failed!");
        return false;
    }

    // Step 2: Configure P2P mode
    // Serial.println("\n[2] Configuring P2P mode...");

    if (!LoRa_SendCommand("AT+NWM=0"))
    {
        // Serial.println("ERROR: P2P mode configuration failed!");
        return false;
    }

    // Step 3: Configure P2P parameters
    // Serial.println("\n[3] Configuring LoRa frequency...");

    char command[80];

    snprintf(command, sizeof(command), "AT+P2P=%lu:7:0:1:8:10", (unsigned long)frequency);

    if (!LoRa_SendCommand(command))
    {
        Serial.println("ERROR: LoRa P2P configuration failed!");
        return false;
    }

    Serial.println("\nRAK3172 initialized successfully!");

    return true;
}

bool LoRa_sendPacket(const uint8_t *data, uint16_t len)
{
    if (data == nullptr || len == 0 || len > 255)
    {
        Serial.println("Invalid LoRa packet!");
        return false;
    }

    // 9 characters for prefix + 510 hex characters + null
    char command[520];

    uint16_t index = snprintf(command, sizeof(command), "AT+PSEND=");

    // Convert binary data into hexadecimal
    for (uint16_t i = 0; i < len; i++)
    {
        int written = snprintf(command + index, sizeof(command) - index, "%02X", data[i]);

        if (written != 2)
        {
            Serial.println("LoRa packet formatting failed!");
            return false;
        }

        index += written;
    }

    // Send packet
    return LoRa_SendCommand(command);
}