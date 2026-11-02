#include "LoRa_app.h"
#include "SPI_Packet.h"
#include "NetworkManager_app.h"

TaskHandle_t LoRaAppHandle;

SemaphoreHandle_t LoRa_ON_FLAG = NULL;
SemaphoreHandle_t LoRa_OFF_FLAG = NULL;
SemaphoreHandle_t LoRa_READY_FLAG = NULL;

uint8_t LoRa_State = LORA_IDLE;
static SensorPacket packet;

void LoRa_app_Init(void)
{
    LoRa_ON_FLAG = xSemaphoreCreateBinary();
    LoRa_OFF_FLAG = xSemaphoreCreateBinary();
    LoRa_READY_FLAG = xSemaphoreCreateBinary();
}

void LoRa_app(void *pvParameters)
{

    while (1)
    {
        switch (LoRa_State)
        {
        case LORA_IDLE:
            if (xSemaphoreTake(LoRa_ON_FLAG, portMAX_DELAY) == pdTRUE)
            {
                xSemaphoreTake(LoRa_OFF_FLAG, 0);
                Serial.println("LoRa: STARTING");
                LoRa_State = LORA_STARTING;
            }
            break;

        case LORA_STARTING:
            if (LoRa_init(LORA_FREQ))
            {
                Serial.println("LoRa Initialized successfully");
                Serial.println("LoRa: READY");
                xSemaphoreGive(LoRa_READY_FLAG);
                LoRa_State = LORA_READY;
            }
            else
            {
                Serial.println("LoRa initialization failed ");
                LoRa_State = LORA_ERROR;
            }
            break;

        case LORA_READY:
            if (xSemaphoreTake(LoRa_OFF_FLAG, 0) == pdTRUE) // Check OFF request
            {
                LoRa_State = LORA_OFF;
                break;
            }


            if (xQueueReceive(sensorData_from_SPI, &packet, pdMS_TO_TICKS(100)) == pdPASS) // Receive sensor packet from SPI queue
            {
                if (xSemaphoreTake(LoRa_OFF_FLAG, 0) == pdTRUE) // Check route and OFF request again
                {
                    xQueueSendToFront(sensorData_from_SPI, &packet, 0);
                    LoRa_State = LORA_OFF;
                    break;
                }

                if (!NetworkManager_IsRouteSelected(NETWORK_LORA))
                {
                    xQueueSendToFront(sensorData_from_SPI, &packet, 0);
                    break;
                }

                //Serial.printf("LoRa: Packet %d received from SPI queue\n", packet.packet_id);

                if (LoRa_sendPacket((const uint8_t *)&packet, sizeof(SensorPacket))) // Send packet through LoRa driver
                {
                    Serial.printf("LoRa: Packet %d sent successfully\n", packet.packet_id);
                }
                else
                {
                    Serial.printf("LoRa: Packet %d send failed\n", packet.packet_id);

                    xQueueSendToFront(sensorData_from_SPI, &packet, 0); // Return packet to queue for retry
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
            }

            break;

        case LORA_OFF:
            Serial.println("LoRa: OFF");

            xSemaphoreTake(LoRa_READY_FLAG, 0); // Clear READY notification
            xSemaphoreTake(LoRa_ON_FLAG, 0);    // Discard ON request received during shutdown

            Serial.println("LoRa: IDLE");

            LoRa_State = LORA_IDLE;
            break;

        case LORA_ERROR:

            Serial.println("LoRa initialization failed.");
            Serial.println("LoRa: Retrying in 3 seconds");

            vTaskDelay(pdMS_TO_TICKS(3000));

            LoRa_State = LORA_STARTING;

            break;

        default:

            LoRa_State = LORA_IDLE;

            break;
        }
         vTaskDelay(pdMS_TO_TICKS(100));
    }
}
