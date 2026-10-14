#ifndef MQTT_HAL_H
#define MQTT_HAL_H

#include <Client.h>

// Which network the MQTT session should use right now
typedef enum {
    MQTT_TRANSPORT_NONE = 0,
    MQTT_TRANSPORT_WIFI,
    MQTT_TRANSPORT_SIM
} MQTT_Transport;

void MQTT_HAL_Init();

// MANUAL choice: which network you WANT MQTT to use (WIFI or SIM).
void MQTT_HAL_SetPreferred(MQTT_Transport transport);
MQTT_Transport MQTT_HAL_GetPreferred();

// Network MQTT should use right now:
//   preferred network if it is connected, else the other one if it is connected, else NONE.
MQTT_Transport MQTT_HAL_GetBestTransport();

// TCP client stream that belongs to a transport (nullptr for NONE)
Client* MQTT_HAL_GetClient(MQTT_Transport transport);

// Kept for compatibility: client of the best transport
Client* MQTT_HAL_GetActiveClient();

#endif // MQTT_HAL_H
