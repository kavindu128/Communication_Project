#include "MQTT_hal.h"
#include "SIM7600_driver.h"
#include "WIFI_driver.h"

void MQTT_HAL_Init() {

}

Client* MQTT_HAL_GetActiveClient() {
    // 1. Prioritize WiFi if it is connected
    if (WIFI_Driver_IsConnected()) {
        return WIFI_Driver_GetClient();
    } 
    // 2. Fall back to Cellular if WiFi is down
    else if (SIM7600_Driver_IsConnected()) {
        return SIM7600_Driver_GetClient();
    }
    
    return nullptr; // No network available
}