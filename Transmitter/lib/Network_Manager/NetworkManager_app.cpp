#include "NetworkManager_app.h"
#include "MQTT_app.h"
#include "WIFI_app.h"
#include "SIM7080_app.h"
#include "SIM7600_app.h"
#include "LoRa_app.h"

TaskHandle_t NetworkManagerAppHandle = nullptr;

volatile NetworkRoute NetworkManager_SelectedRoute = NETWORK_NONE;

static volatile bool wifiEnabled = false;
static volatile bool sim7600Enabled = false;
static volatile bool sim7080Enabled = false;
static volatile bool loraEnabled = false;

static uint32_t routeSelectedAt = 0;
static uint32_t routeCooldownUntil[5] = {0, 0, 0, 0, 0};

static const uint32_t WIFI_LINK_TIMEOUT_MS = 20000;
static const uint32_t CELL_LINK_TIMEOUT_MS = 60000;
static const uint32_t MQTT_CONNECT_TIMEOUT_MS = 20000;
static const uint32_t ROUTE_COOLDOWN_MS = 30000;

static bool routeEnabled(NetworkRoute route)
{
    switch (route)
    {
    case NETWORK_WIFI:
        return wifiEnabled;
    case NETWORK_SIM7600:
        return sim7600Enabled;
    case NETWORK_SIM7080:
        return sim7080Enabled;
    case NETWORK_LORA:
        return loraEnabled;
    default:
        return false;
    }
}

static bool routeIsPhysicallyReady(NetworkRoute route)
{
    switch (route)
    {
    case NETWORK_WIFI:
        return WIFI_State == WIFI_STATE_CONNECTED && WIFI_Driver_IsConnected();
    case NETWORK_SIM7600:
        return SIM7600_State == SIM7600_CONNECTED && SIM7600_Driver_IsLinkUp();
    case NETWORK_SIM7080:
        return SIM7080_State == SIM7080_CONNECTED && SIM7080_Driver_IsLinkUp();
    case NETWORK_LORA:
        return LoRa_State == LORA_READY;
    default:
        return false;
    }
}

static int routePriorityRank(NetworkRoute route)
{
    switch (route)
    {
    case NETWORK_WIFI:
        return 0;
    case NETWORK_SIM7600:
        return 1;
    case NETWORK_SIM7080:
        return 2;
    case NETWORK_LORA:
        return 3;
    default:
        return 4;
    }
}

static MQTT_Transport mqttTransportForRoute(NetworkRoute route)
{
    switch (route)
    {
    case NETWORK_WIFI:
        return MQTT_TRANSPORT_WIFI;
    case NETWORK_SIM7600:
        return MQTT_TRANSPORT_SIM7600;
    case NETWORK_SIM7080:
        return MQTT_TRANSPORT_SIM7080;
    default:
        return MQTT_TRANSPORT_NONE;
    }
}

static uint32_t routeLinkTimeout(NetworkRoute route)
{
    return route == NETWORK_WIFI ? WIFI_LINK_TIMEOUT_MS : CELL_LINK_TIMEOUT_MS;
}

static void requestRoutePower(NetworkRoute route, bool state)
{
    switch (route)
    {
    case NETWORK_WIFI:
        if (state && WIFI_ON_FLAG)
            xSemaphoreGive(WIFI_ON_FLAG);
        else if (!state && WIFI_OFF_FLAG)
            xSemaphoreGive(WIFI_OFF_FLAG);
        break;
    case NETWORK_SIM7600:
        if (state && SIM7600_ON_FLAG)
            xSemaphoreGive(SIM7600_ON_FLAG);
        else if (!state && SIM7600_OFF_FLAG)
            xSemaphoreGive(SIM7600_OFF_FLAG);
        break;
    case NETWORK_SIM7080:
        if (state && SIM7080_ON_FLAG)
            xSemaphoreGive(SIM7080_ON_FLAG);
        else if (!state && SIM7080_OFF_FLAG)
            xSemaphoreGive(SIM7080_OFF_FLAG);
        break;
    case NETWORK_LORA:
        if (state && LoRa_ON_FLAG)
            xSemaphoreGive(LoRa_ON_FLAG);
        else if (!state && LoRa_OFF_FLAG)
            xSemaphoreGive(LoRa_OFF_FLAG);
        break;
    default:
        break;
    }
}

void NetworkManager_HandleCommand(NetworkRoute route, bool state)
{
    if (route < NETWORK_WIFI || route > NETWORK_LORA)
    {
        Serial.println("Network Manager: invalid route command");
        return;
    }

    bool wasEnabled = routeEnabled(route);
    if (wasEnabled == state)
    {
        Serial.printf("Network Manager: %s is already %s\n",
                      route == NETWORK_WIFI ? "WiFi" : route == NETWORK_SIM7600 ? "SIM7600"
                                                   : route == NETWORK_SIM7080   ? "SIM7080"
                                                                                : "LoRa",
                      state ? "enabled" : "disabled");
        return;
    }

    switch (route)
    {
    case NETWORK_WIFI:
        wifiEnabled = state;
        break;
    case NETWORK_SIM7600:
        sim7600Enabled = state;
        break;
    case NETWORK_SIM7080:
        sim7080Enabled = state;
        break;
    case NETWORK_LORA:
        loraEnabled = state;
        break;
    default:
        return;
    }

    if (state)
    {
        // A user ON command allows this route to be tried again immediately.
        routeCooldownUntil[(int)route] = 0;
        requestRoutePower(route, true);
    }
    else
    {
        requestRoutePower(route, false);
        if (NetworkManager_SelectedRoute == route)
        {
            NetworkManager_SelectedRoute = NETWORK_NONE;
            MQTT_HAL_SetPreferred(MQTT_TRANSPORT_NONE);
            MQTT_App_Connected = false;
        }
    }

    Serial.printf("Network Manager: %s %s\n",
                  route == NETWORK_WIFI ? "WiFi" : route == NETWORK_SIM7600 ? "SIM7600"
                                               : route == NETWORK_SIM7080   ? "SIM7080"
                                                                            : "LoRa",
                  state ? "enabled" : "disabled");
}

static NetworkRoute chooseRoute()
{
    const NetworkRoute priority[] = { NETWORK_WIFI, NETWORK_SIM7600, NETWORK_SIM7080 };
    const uint32_t now = millis();

    // 1. Best enabled route that is physically ready
    for (NetworkRoute route : priority)
    {
        if (routeEnabled(route) &&
            (int32_t)(now - routeCooldownUntil[(int)route]) >= 0 &&
            routeIsPhysicallyReady(route))
        {
            return route;
        }
    }

    // 2. Nothing else is ready: fall back to LoRa
    if (loraEnabled && LoRa_State == LORA_READY)
        return NETWORK_LORA;

    // 3. LoRa unavailable: let the best enabled route keep trying to connect
    for (NetworkRoute route : priority)
    {
        if (routeEnabled(route) &&
            (int32_t)(now - routeCooldownUntil[(int)route]) >= 0)
        {
            return route;
        }
    }

    return NETWORK_NONE;
}

static void updateActiveRoute()
{
    const uint32_t now = millis();
    NetworkRoute current = NetworkManager_SelectedRoute;

    // If the active route is unavailable for too long, cool it down and try the next one.
    if (current != NETWORK_NONE && current != NETWORK_LORA && routeEnabled(current))
    {
        uint32_t elapsed = now - routeSelectedAt;
        bool physicalReady = routeIsPhysicallyReady(current);
        uint32_t allowedTime = physicalReady ? MQTT_CONNECT_TIMEOUT_MS : routeLinkTimeout(current);

        if (elapsed >= allowedTime && !MQTT_App_Connected)
        {
            routeCooldownUntil[(int)current] = now + ROUTE_COOLDOWN_MS;
            Serial.printf("Network Manager: %d unavailable; trying next route\n", (int)current);
        }
    }

    NetworkRoute requested = chooseRoute();

    // Keep a healthy route unless a higher-priority route has recovered.
    if (current != NETWORK_NONE && current != NETWORK_LORA && MQTT_App_Connected && MQTT_App_BoundTransport == mqttTransportForRoute(current) && routeEnabled(current) && routeIsPhysicallyReady(current) && (int32_t)(now - routeCooldownUntil[(int)current]) >= 0 && routePriorityRank(requested) > routePriorityRank(current))
    {
        requested = current;
    }

    if (requested == current)
        return;

    NetworkManager_SelectedRoute = requested;
    routeSelectedAt = now;
    MQTT_HAL_SetPreferred(mqttTransportForRoute(requested));

    const char *name = requested == NETWORK_WIFI ? "WiFi" : requested == NETWORK_SIM7600 ? "SIM7600"
                                                        : requested == NETWORK_SIM7080   ? "SIM7080"
                                                        : requested == NETWORK_LORA      ? "LoRa"
                                                                                         : "NONE";
    Serial.printf("Network Manager: active route -> %s\n", name);
}

static NetworkRoute routeForOnCommand(char command)
{
    switch (command)
    {
    case '1':
        return NETWORK_WIFI;
    case '3':
        return NETWORK_SIM7080;
    case '5':
        return NETWORK_SIM7600;
    case '7':
        return NETWORK_LORA;
    default:
        return NETWORK_NONE;
    }
}

static NetworkRoute routeForOffCommand(char command)
{
    switch (command)
    {
    case '2':
        return NETWORK_WIFI;
    case '4':
        return NETWORK_SIM7080;
    case '6':
        return NETWORK_SIM7600;
    case '8':
        return NETWORK_LORA;
    default:
        return NETWORK_NONE;
    }
}

void NetworkManager_Init()
{
    Serial.println();
    Serial.println("========== NETWORK MANAGER / AUTO FAILOVER ==========");
    Serial.println("Serial commands (115200 baud):");
    Serial.println("  1/2 = WiFi ON/OFF");
    Serial.println("  5/6 = SIM7600 ON/OFF");
    Serial.println("  3/4 = SIM7080 ON/OFF");
    Serial.println("  7/8 = LoRa ON/OFF");
    Serial.println("  0   = Disable all networks");
    Serial.println("Failover priority: WiFi -> SIM7600 -> SIM7080 -> LoRa");
}

bool NetworkManager_IsRouteSelected(NetworkRoute route)
{
    return NetworkManager_SelectedRoute == route;
}

void NetworkManager_app(void *pvParameters)
{
    NetworkManager_Init();

    for (;;)
    {
        while (Serial.available() > 0)
        {
            char command = (char)Serial.read();
            if (command == '\r' || command == '\n' || command == ' ' || command == '\t')
                continue;

            if (command == '0')
            {
                NetworkManager_HandleCommand(NETWORK_WIFI, false);
                NetworkManager_HandleCommand(NETWORK_SIM7600, false);
                NetworkManager_HandleCommand(NETWORK_SIM7080, false);
                NetworkManager_HandleCommand(NETWORK_LORA, false);
                continue;
            }

            NetworkRoute route = routeForOnCommand(command);
            if (route != NETWORK_NONE)
            {
                NetworkManager_HandleCommand(route, true);
                continue;
            }

            route = routeForOffCommand(command);
            if (route != NETWORK_NONE)
            {
                NetworkManager_HandleCommand(route, false);
                continue;
            }

            Serial.println("Unknown command. Enter 0 to 8 using the menu above.");
        }

        updateActiveRoute();
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
