#include "MQTT_app.h"
                  
TaskHandle_t MQTTAppHandle;

#define MQTT_RETRY_MS          3000    // wait between failed broker connects
#define MQTT_WIFI_TIMEOUT_S    5
#define MQTT_SIM_TIMEOUT_S     30

// Only the 4G client talks AT commands over the UART. WiFi MQTT is a plain
// TCP stream, so it must never wait for (or block) the ModemMutex.
static const char* transportName(MQTT_Transport t) {
    return t == MQTT_TRANSPORT_WIFI ? "WiFi" : (t == MQTT_TRANSPORT_SIM ? "4G" : "NONE");
}

static inline bool needsModemMutex(MQTT_Transport t) {
    return t == MQTT_TRANSPORT_SIM;
}

static bool takeIfNeeded(MQTT_Transport t, TickType_t wait) {
    if (!needsModemMutex(t)) return true;
    if (ModemMutex == NULL) return false;
    return xSemaphoreTake(ModemMutex, wait) == pdTRUE;
}

static void giveIfNeeded(MQTT_Transport t) {
    if (needsModemMutex(t)) xSemaphoreGive(ModemMutex);
}

// Close the old session cleanly (if the old network still exists)
static void teardown(MQTT_Transport old) {
    if (old == MQTT_TRANSPORT_WIFI) {
        MQTT_Driver_Disconnect();                       // no UART involved
    }
    else if (old == MQTT_TRANSPORT_SIM) {
        // If the modem is already gone, SIM7600_Driver_PowerOff() closed the socket.
        // Do not send AT commands to a modem that is off or restarting.
        if (SIM7600_State == SIM7600_CONNECTED && takeIfNeeded(old, pdMS_TO_TICKS(5000))) {
            MQTT_Driver_Disconnect();
            giveIfNeeded(old);
        }
    }
}

void MQTT_App_ProcessCommand(const char* method, bool state)
{
    if (strcmp(method, "SIM7600") == 0)
    {
        if (state)
        {
            xSemaphoreGive(SIM7600_ON_FLAG);
            Serial.println("SIM7600 ON command received");
        }
        else
        {
            xSemaphoreGive(SIM7600_OFF_FLAG);
            Serial.println("SIM7600 OFF command received");
        }
    }

    else if (strcmp(method, "WIFI") == 0)
    {
        if (state)
        {
            xSemaphoreGive(WIFI_ON_FLAG);
            Serial.println("WIFI ON command received");
        }
        else 
        {
            xSemaphoreGive(WIFI_OFF_FLAG);
            Serial.println("WIFI OFF command received");
        }
    }
}

void MQTT_app(void *pvParameters) {

    // No waiting for the SIM here: WiFi may be the only network for a while.
    MQTT_Driver_Init();

    SensorPacket dataToSend;
    MQTT_Transport bound = MQTT_TRANSPORT_NONE;   // network the MQTT session is attached to
    uint32_t nextConnectTry = 0;

    for (;;) {

        // 1. Which network should we be on right now?
        MQTT_Transport want = MQTT_HAL_GetBestTransport();

        // 2. Network changed (WiFi up/down, 4G up/down): move the session
        if (want != bound) {
            Serial.printf("MQTT: %s -> %s\n", transportName(bound), transportName(want));

            if (bound != MQTT_TRANSPORT_NONE) {
                teardown(bound);
            }

            bound = want;
            if (bound != MQTT_TRANSPORT_NONE) {
                MQTT_Driver_BindClient(MQTT_HAL_GetClient(bound),
                                       bound == MQTT_TRANSPORT_WIFI ? MQTT_WIFI_TIMEOUT_S
                                                                    : MQTT_SIM_TIMEOUT_S);
            } else {
                MQTT_Driver_BindClient(nullptr, 0);
            }
            nextConnectTry = millis();                // connect to the new network immediately
        }

        // 3. No network at all: sensor packets simply stay in the queue
        if (bound == MQTT_TRANSPORT_NONE) {
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        // 4. Work on the active network (mutex only if it is the 4G modem)
        if (takeIfNeeded(bound, pdMS_TO_TICKS(5000)))
        {
            if (!MQTT_Driver_IsConnected())
            {
                // Connect only when disconnected
                if ((int32_t)(millis() - nextConnectTry) >= 0)
                {
                    Serial.println("MQTT: Trying to connect...");

                    if (MQTT_Driver_Connect())
                    {
                        Serial.println("MQTT: Connected");

                        if (MQTT_Driver_SubscribeCommands())
                        {
                            Serial.println("MQTT: RPC subscribed");
                        }
                        else
                        {
                            Serial.println("MQTT: RPC subscription failed");
                        }

                        nextConnectTry = millis() + MQTT_RETRY_MS;
                    }
                    else
                    {
                        Serial.println("MQTT: Connection failed");
                        nextConnectTry = millis() + MQTT_RETRY_MS;
                    }
                }
            }
            else
            {
                // MQTT is connected: process incoming commands
                MQTT_Driver_Loop();

                // Read sensor data without removing it
                if (xQueuePeek(sensorData_from_SPI, &dataToSend, 0) == pdPASS)
                {
                    char jsonPayload[256];

                    snprintf(jsonPayload, sizeof(jsonPayload),
                        "{\"packet_id\":%u,\"water_ph\":%.2f,"
                        "\"turbidity\":%.2f,\"soil_moisture\":%.2f,"
                        "\"soil_ph\":%.2f}",
                        dataToSend.packet_id,
                        dataToSend.water_ph,
                        dataToSend.turbidity,
                        dataToSend.soil_moisture,
                        dataToSend.soil_ph);

                    Serial.printf("MQTT payload: %s\n", jsonPayload);

                    if (MQTT_Driver_Publish(
                            "v1/devices/me/telemetry", jsonPayload))
                    {
                        // Remove data only after successful publishing
                        xQueueReceive(sensorData_from_SPI, &dataToSend, 0);

                        Serial.printf("Published SPI data via %s.\n",
                            bound == MQTT_TRANSPORT_WIFI ? "WiFi" : "4G");
                    }
                    else
                    {
                        Serial.println("Publish failed, will retry.");
                    }
                }
            }

            giveIfNeeded(bound);
        }

    vTaskDelay(pdMS_TO_TICKS(20));

    }
}


