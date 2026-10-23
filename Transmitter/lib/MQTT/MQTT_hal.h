#ifndef MQTT_HAL_H
#define MQTT_HAL_H

#include "SIM7600_app.h"
#include "SIM7600_driver.h"
#include "SIM7080_app.h"
#include "SIM7080_driver.h"
#include "WIFI_driver.h"

#include <Client.h>

// Which network the MQTT session should use right now
typedef enum {
    MQTT_TRANSPORT_NONE = 0,
    MQTT_TRANSPORT_WIFI,
    MQTT_TRANSPORT_SIM7600,
    MQTT_TRANSPORT_SIM7080

} MQTT_Transport;

void MQTT_HAL_Init();

// MANUAL choice: which network you WANT MQTT to use (WIFI or SIM).
void MQTT_HAL_SetPreferred(MQTT_Transport transport);
MQTT_Transport MQTT_HAL_GetPreferred();

// Returns the Network Manager preferred route only when its network link is ready.
// Network Manager chooses the route and performs automatic failover.
MQTT_Transport MQTT_HAL_GetBestTransport();
Client* MQTT_HAL_GetClient(MQTT_Transport transport);
Client* MQTT_HAL_GetActiveClient();

#endif // MQTT_HAL_H
