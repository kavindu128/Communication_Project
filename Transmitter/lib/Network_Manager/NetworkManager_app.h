#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>

extern TaskHandle_t NetworkManagerAppHandle;

// One selected route at a time.
typedef enum
{
    NETWORK_NONE = 0,
    NETWORK_WIFI,
    NETWORK_SIM7080,
    NETWORK_SIM7600,
    NETWORK_LORA
} NetworkRoute;

extern volatile NetworkRoute NetworkManager_SelectedRoute;

void NetworkManager_Init();
void NetworkManager_app(void *pvParameters);
bool NetworkManager_IsRouteSelected(NetworkRoute route);
bool NetworkManager_IsRouteSelected(NetworkRoute route);
void NetworkManager_HandleCommand(NetworkRoute route, bool state);

#endif