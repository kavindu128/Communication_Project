#include "MQTT_hal.h"
#include "SIM7600_app.h"
#include "SIM7600_driver.h"
#include "WIFI_driver.h"

void MQTT_HAL_Init() {

}

static volatile MQTT_Transport preferredTransport = MQTT_TRANSPORT_WIFI;

void MQTT_HAL_SetPreferred(MQTT_Transport transport) {
    if (transport == MQTT_TRANSPORT_WIFI || transport == MQTT_TRANSPORT_SIM) {
        preferredTransport = transport;
    }
}

MQTT_Transport MQTT_HAL_GetPreferred() {
    return preferredTransport;
}

// WiFi.status() is a plain memory read - no UART
static bool wifiReady() {
    return WIFI_Driver_IsConnected();
}

// 4G is usable only when the SIM task says it is fully up AND the cached link flag is set.
// No AT command here: while the modem is starting, the UART belongs to the SIM task.
static bool simReady() {
    return SIM7600_State == SIM7600_CONNECTED && SIM7600_Driver_IsLinkUp();
}

MQTT_Transport MQTT_HAL_GetBestTransport() {
    bool wifi = wifiReady();
    bool sim  = simReady();

    if (preferredTransport == MQTT_TRANSPORT_SIM) {
        if (sim)  return MQTT_TRANSPORT_SIM;
        if (wifi) return MQTT_TRANSPORT_WIFI;      // 4G not ready yet -> keep working on WiFi
    } else {
        if (wifi) return MQTT_TRANSPORT_WIFI;
        if (sim)  return MQTT_TRANSPORT_SIM;       // WiFi not ready yet -> keep working on 4G
    }
    return MQTT_TRANSPORT_NONE;
}

Client* MQTT_HAL_GetClient(MQTT_Transport transport) {
    switch (transport) {
        case MQTT_TRANSPORT_WIFI: return WIFI_Driver_GetClient();
        case MQTT_TRANSPORT_SIM:  return SIM7600_Driver_GetClient();
        default:                  return nullptr;
    }
}

Client* MQTT_HAL_GetActiveClient() {
    return MQTT_HAL_GetClient(MQTT_HAL_GetBestTransport());
}
