#include "WIFI_driver.h"
#include <WiFi.h>

static WiFiClient wifiClient;

void WIFI_Driver_Init() {
    // Set WiFi to station mode (client) and disconnect from any previous networks
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    vTaskDelay(pdMS_TO_TICKS(100));
}

void WIFI_Driver_Connect(const char* ssid, const char* password) {
    Serial.printf("Connecting to SSID: %s\n", ssid);
    WiFi.begin(ssid, password);
}

void WIFI_Driver_Disconnect() {
    WiFi.disconnect(false);
}

bool WIFI_Driver_IsConnected() {
    if (WiFi.getMode() == WIFI_MODE_NULL) {
        return false; // Radio is off, do not poll WiFi.status()
    }
    
    // 2. Only poll the status if the radio is active
    return WiFi.status() == WL_CONNECTED;
}

void WIFI_Driver_PowerOff() {
    wifiClient.stop();
    WiFi.disconnect(true);      // leave the AP and turn the station off
    WiFi.mode(WIFI_OFF);        // radio fully off
}

void WIFI_Driver_PrintIP() {
    Serial.print("WiFi Connected. IP Address: ");
    Serial.println(WiFi.localIP());
}

Client* WIFI_Driver_GetClient() {
    return &wifiClient;
}