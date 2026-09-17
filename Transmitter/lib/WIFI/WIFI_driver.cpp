#include "WIFI_driver.h"
#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID       "ESP32_SENSOR"
#define WIFI_PASSWORD   "12345678"
#define WIFI_PORT       5000

WiFiServer server(WIFI_PORT);
WiFiClient client;


// =====================================
// WiFi Initialization
// =====================================

bool WIFI_Init()
{
    Serial.println("Starting WiFi AP...");

    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD))
    {
        Serial.println("WiFi AP creation failed");
        return false;
    }

    Serial.println("WiFi AP created successfully");

    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    server.begin();

    Serial.print("TCP Server Port: ");
    Serial.println(WIFI_PORT);

    return true;
}


// WiFi Send


bool WIFI_SendData(
    const uint8_t *data,
    size_t len
)
{

    // Check connected client
    {
    if (data == NULL || len == 0)
    {
        return false;
    }

    // Check for new client
    if (!client || !client.connected())
    {
        WiFiClient newClient = server.available();

        if (newClient)
        {
            client = newClient;

            Serial.println("WiFi client connected");
        }
        else
        {
            return false;
        }
    }

    // Send packet
    size_t sent = client.write(
        data,
        len
    );

    if (sent == len)
    {
        // Serial.print("WiFi packet sent: ");
        // Serial.print(sent);
        // Serial.println(" bytes");

        return true;
    }

    Serial.println("WiFi packet send failed");

    return false;
    }


}
// WiFi Receive


bool WIFI_ReceiveData(
    uint8_t *data,
    size_t len
)
{
    // Check data
    if (data == NULL || len == 0)
    {
        return false;
    }


    // Check connected client
    if (!client || !client.connected())
    {
        client = server.available();

        if (!client)
        {
            return false;
        }

        Serial.println("WiFi client connected");
    }


    // Check if enough data is available
    if (client.available() >= len)
    {
        size_t received = client.read(data, len);

        if (received == len)
        {
            Serial.print("WiFi received: ");
            Serial.print(received);
            Serial.println(" bytes");

            return true;
        }
        else
        {
            Serial.println("WiFi receive failed");

            return false;
        }
    }


    // Data is not available yet
    return false;
}

