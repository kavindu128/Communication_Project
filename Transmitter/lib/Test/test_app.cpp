#include "test_app.h"

TaskHandle_t TestHandle;

// same numbers for SIM7600_* and WIFI_STATE_*
static const char* names[] = {"IDLE", "OFF", "STARTING", "CONNECTING", "CONNECTED"};

void Test_app(void * pvParameters) {

    Serial.println("Test ready:  O/X = SIM on/off   W/Q = WiFi on/off   B/Z = both on/off");

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
