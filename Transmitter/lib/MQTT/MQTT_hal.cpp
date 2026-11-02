#include "MQTT_hal.h"

void MQTT_HAL_Init()
{
}

static volatile MQTT_Transport preferredTransport = MQTT_TRANSPORT_NONE;

void MQTT_HAL_SetPreferred(MQTT_Transport transport)
{
    if (transport == MQTT_TRANSPORT_WIFI || transport == MQTT_TRANSPORT_SIM7600 || transport == MQTT_TRANSPORT_SIM7080)
    {
        preferredTransport = transport;
    }
}

MQTT_Transport MQTT_HAL_GetBestTransport()
{
    switch (preferredTransport)
    {
    case MQTT_TRANSPORT_WIFI:
        return WIFI_Driver_IsConnected() ? MQTT_TRANSPORT_WIFI : MQTT_TRANSPORT_NONE;

    case MQTT_TRANSPORT_SIM7080:
        return (SIM7080_State == SIM7080_CONNECTED && SIM7080_Driver_IsLinkUp())
                   ? MQTT_TRANSPORT_SIM7080
                   : MQTT_TRANSPORT_NONE;

    case MQTT_TRANSPORT_SIM7600:
        return (SIM7600_State == SIM7600_CONNECTED && SIM7600_Driver_IsLinkUp())
                   ? MQTT_TRANSPORT_SIM7600
                   : MQTT_TRANSPORT_NONE;

    default:
        return MQTT_TRANSPORT_NONE;
    }
}

Client *MQTT_HAL_GetClient(MQTT_Transport transport)
{
    switch (transport)
    {
    case MQTT_TRANSPORT_WIFI:
        return WIFI_Driver_GetClient();
    case MQTT_TRANSPORT_SIM7080:
        return SIM7080_Driver_GetClient();
    case MQTT_TRANSPORT_SIM7600:
        return SIM7600_Driver_GetClient();
    default:
        return nullptr;
    }
}

Client *MQTT_HAL_GetActiveClient()
{
    return MQTT_HAL_GetClient(MQTT_HAL_GetBestTransport());
}
