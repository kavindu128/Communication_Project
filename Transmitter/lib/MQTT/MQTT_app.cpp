#include "MQTT_app.h"
#include "NetworkManager_app.h"

TaskHandle_t MQTTAppHandle;

#define MQTT_RETRY_MS 3000 // wait between failed broker connects
#define MQTT_WIFI_TIMEOUT_S 5
#define MQTT_CELL_TIMEOUT_S 30

volatile MQTT_Transport MQTT_App_BoundTransport = MQTT_TRANSPORT_NONE;
volatile bool MQTT_App_Connected = false;

static const char *transportName(MQTT_Transport t)
{
    switch (t)
    {
    case MQTT_TRANSPORT_WIFI:
        return "WiFi";
    case MQTT_TRANSPORT_SIM7080:
        return "SIM7080";
    case MQTT_TRANSPORT_SIM7600:
        return "SIM7600";
    default:
        return "NONE";
    }
}

static SemaphoreHandle_t transportMutex(MQTT_Transport t)
{
    if (t == MQTT_TRANSPORT_SIM7080)
        return Sim7080ModemMutex;
    if (t == MQTT_TRANSPORT_SIM7600)
        return Sim7600ModemMutex;
    return nullptr;
}

static bool takeTransport(MQTT_Transport t, TickType_t wait)
{
    SemaphoreHandle_t mutex = transportMutex(t);
    return mutex == nullptr || xSemaphoreTake(mutex, wait) == pdTRUE;
}

static void giveTransport(MQTT_Transport t)
{
    SemaphoreHandle_t mutex = transportMutex(t);
    if (mutex != nullptr)
        xSemaphoreGive(mutex);
}

static void closeBoundTransport(MQTT_Transport old)
{
    if (old == MQTT_TRANSPORT_NONE)
        return;

    if (old == MQTT_TRANSPORT_WIFI)
    {
        MQTT_Driver_Disconnect();
        return;
    }

    // Don't send AT commands if the modem is no longer connected/ready.
    bool ready = (old == MQTT_TRANSPORT_SIM7080)
                     ? (SIM7080_State == SIM7080_CONNECTED)
                     : (SIM7600_State == SIM7600_CONNECTED);

    if (ready && takeTransport(old, pdMS_TO_TICKS(3000)))
    {
        MQTT_Driver_Disconnect();
        giveTransport(old);
    }
}

void MQTT_App_ProcessCommand(const char *method, bool state)
{
    if (strcmp(method, "SIM7600") == 0)
        NetworkManager_HandleCommand(NETWORK_SIM7600, state);
    else if (strcmp(method, "SIM7080") == 0)
        NetworkManager_HandleCommand(NETWORK_SIM7080, state);
    else if (strcmp(method, "WIFI") == 0)
        NetworkManager_HandleCommand(NETWORK_WIFI, state);
    else if (strcmp(method, "LORA") == 0)
        NetworkManager_HandleCommand(NETWORK_LORA, state);
    else
    {
        Serial.printf("Unknown MQTT RPC method: %s\n", method);
        return;
    }

    Serial.printf("MQTT RPC: %s %s command handled by Network Manager\n",
                  method, state ? "ON" : "OFF");
}

void MQTT_app(void *pvParameters)
{

    // No waiting for the SIM here: WiFi may be the only network for a while.
    MQTT_Driver_Init();

    SensorPacket dataToSend;
    MQTT_Transport bound = MQTT_TRANSPORT_NONE; // network the MQTT session is attached to
    uint32_t nextConnectTry = 0;

    for (;;)
    {
        // This returns only the route explicitly selected in Network Manager.
        MQTT_Transport wanted = MQTT_HAL_GetBestTransport();

        if (wanted != bound)
        {
            Serial.printf("MQTT route: %s -> %s\n", transportName(bound), transportName(wanted));

            closeBoundTransport(bound);
            bound = wanted;
            MQTT_App_BoundTransport = bound;
            MQTT_App_Connected = false;

            if (bound != MQTT_TRANSPORT_NONE)
            {
                uint16_t timeout = (bound == MQTT_TRANSPORT_WIFI)
                                       ? MQTT_WIFI_TIMEOUT_S
                                       : MQTT_CELL_TIMEOUT_S;
                MQTT_Driver_BindClient(MQTT_HAL_GetClient(bound), timeout);
            }
            else
            {
                MQTT_Driver_Disconnect();
                MQTT_Driver_BindClient(nullptr, 0);
                MQTT_App_Connected = false;
            }

            nextConnectTry = millis();
        }

        if (bound == MQTT_TRANSPORT_NONE)
        {
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        if (!takeTransport(bound, pdMS_TO_TICKS(1000)))
        {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        if (!MQTT_Driver_IsConnected())
        {
            if ((int32_t)(millis() - nextConnectTry) >= 0)
            {
                MQTT_App_Connected = false;
                Serial.printf("MQTT: connecting over %s...\n", transportName(bound));
                if (MQTT_Driver_Connect())
                {
                    Serial.println("MQTT connected");
                    MQTT_App_Connected = true;
                    if (MQTT_Driver_SubscribeCommands())
                        Serial.println("MQTT RPC subscribed");
                    else
                        Serial.println("MQTT RPC subscription failed");
                }
                else
                {
                    Serial.println("MQTT connection failed");
                }
                nextConnectTry = millis() + MQTT_RETRY_MS;
            }
        }
        else
        {
            MQTT_Driver_Loop();

            // Peek first; remove from queue only after successful publish.
            if (xQueuePeek(sensorData_from_SPI, &dataToSend, 0) == pdPASS)
            {
                char payload[256];
                snprintf(payload, sizeof(payload),
                         "{\"packet_id\":%u,\"water_ph\":%.2f,"
                         "\"turbidity\":%.2f,\"soil_moisture\":%.2f,"
                         "\"soil_ph\":%.2f}",
                         (unsigned)dataToSend.packet_id,
                         dataToSend.water_ph,
                         dataToSend.turbidity,
                         dataToSend.soil_moisture,
                         dataToSend.soil_ph);

                if (MQTT_Driver_Publish("v1/devices/me/telemetry", payload))
                {
                    xQueueReceive(sensorData_from_SPI, &dataToSend, 0);
                    Serial.printf("Sensor packet published through %s\n", transportName(bound));
                }
                else
                {
                    Serial.println("MQTT publish failed; packet retained for retry");
                }
            }
        }

        giveTransport(bound);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
