#include "test_app.h"
#include "MQTT_hal.h"

TaskHandle_t TestHandle;

// same numbers for SIM7600_* and WIFI_STATE_*
static const char* names[] = {"IDLE", "OFF", "STARTING", "CONNECTING", "CONNECTED"};

void Test_app(void * pvParameters) {

    Serial.println("Test ready:  O/X = SIM on/off   W/Q = WiFi on/off   B/Z = both on/off   G/F = MQTT via 4G/WiFi   T = status");

    uint8_t lastSim  = 255;
    uint8_t lastWifi = 255;

    while (1) {

        // 1. read the command
        if (Serial.available()) {
            char cmd = toupper(Serial.read());

            if      (cmd == 'O') { Serial.println("Command: SIM ON");   xSemaphoreGive(SIM7600_ON_FLAG);  }
            else if (cmd == 'X') { Serial.println("Command: SIM OFF");  xSemaphoreGive(SIM7600_OFF_FLAG); }
            else if (cmd == 'W') { Serial.println("Command: WIFI ON");  xSemaphoreGive(WIFI_ON_FLAG);     }
            else if (cmd == 'Q') { Serial.println("Command: WIFI OFF"); xSemaphoreGive(WIFI_OFF_FLAG);    }
            else if (cmd == 'B') { Serial.println("Command: BOTH ON");
                                   xSemaphoreGive(SIM7600_ON_FLAG);  xSemaphoreGive(WIFI_ON_FLAG);  }
            else if (cmd == 'Z') { Serial.println("Command: BOTH OFF");
                                   xSemaphoreGive(SIM7600_OFF_FLAG); xSemaphoreGive(WIFI_OFF_FLAG); }
            else if (cmd == 'G') {
                Serial.println("Command: MQTT -> 4G (WiFi keeps sending until 4G is connected)");
                MQTT_HAL_SetPreferred(MQTT_TRANSPORT_SIM);
                if (SIM7600_State == SIM7600_IDLE) xSemaphoreGive(SIM7600_ON_FLAG);   // start 4G if it is off
            }
            else if (cmd == 'F') {
                Serial.println("Command: MQTT -> WiFi (4G keeps sending until WiFi is connected)");
                MQTT_HAL_SetPreferred(MQTT_TRANSPORT_WIFI);
                if (WIFI_State == WIFI_STATE_IDLE) xSemaphoreGive(WIFI_ON_FLAG);       // start WiFi if it is off
            }
            else if (cmd == 'T') {
                MQTT_Transport pref = MQTT_HAL_GetPreferred();
                MQTT_Transport use  = MQTT_HAL_GetBestTransport();
                Serial.printf("Status: preferred=%s  using=%s  SIM=%s  WIFI=%s\n",
                              pref == MQTT_TRANSPORT_SIM ? "4G" : "WiFi",
                              use == MQTT_TRANSPORT_SIM ? "4G" : (use == MQTT_TRANSPORT_WIFI ? "WiFi" : "NONE"),
                              names[SIM7600_State], names[WIFI_State]);
            }
        }

        // 2. print every state change of both machines
        if (SIM7600_State != lastSim) {
            lastSim = SIM7600_State;
            Serial.printf("   SIM  -> %s\n", names[lastSim]);
        }
        if (WIFI_State != lastWifi) {
            lastWifi = WIFI_State;
            Serial.printf("   WIFI -> %s\n", names[lastWifi]);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
