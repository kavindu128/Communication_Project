#include "MQTT_app.h"
                  
TaskHandle_t MQTTAppHandle;

void MQTT_app(void *pvParameters) {
    while (SIM7600_State != SIM7600_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
    }

    xSemaphoreTake(ModemMutex, portMAX_DELAY);
    MQTT_Driver_Init();
    xSemaphoreGive(ModemMutex);

    sensor_packet dataToSend; 

    for (;;) {
        // Note: SIM7600_Driver_IsConnected() is safe outside the Mutex 
        // because you updated it to return a cached memory variable.
        if (WIFI_Driver_IsConnected() || SIM7600_State == SIM7600_CONNECTED) {
            
            // 1. TAKE MUTEX: Wait up to 5 seconds for the physical UART lines
            if (xSemaphoreTake(ModemMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
                
                if (!MQTT_Driver_IsConnected()) {
                    if (!MQTT_Driver_Connect()) {
                        Serial.println("MQTT Connection failed. Resetting SIM flag...");
        
                        vTaskDelay(pdMS_TO_TICKS(2000));
                    }
                } else {
                    // These functions trigger AT commands. They are now protected.
                    MQTT_Driver_Loop(); 

                    if (xQueueReceive(sensorData_to_WiFi_Queue, &dataToSend, pdMS_TO_TICKS(10)) == pdPASS) {
                        char jsonPayload[256];
                        snprintf(jsonPayload, sizeof(jsonPayload), 
                            "{\"packet_id\":%u, \"water_ph\":%.2f, \"turbidity\":%.2f, \"soil_moisture\":%.2f, \"soil_ph\":%.2f}", 
                            dataToSend.packet_id, dataToSend.water_ph, dataToSend.turbidity, 
                            dataToSend.soil_moisture, dataToSend.soil_ph
                        );

                        if (MQTT_Driver_Publish("v1/devices/me/telemetry", jsonPayload)) {
                            Serial.println("Published SPI data to ThingsBoard.");
                            vTaskDelay(pdMS_TO_TICKS(200)); 
                        } else {
                            Serial.println("Publish failed.");
                        }
                    }
                }
                
                // 2. GIVE MUTEX: Always return the key so the SIM task isn't permanently locked out
                xSemaphoreGive(ModemMutex);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
   

}