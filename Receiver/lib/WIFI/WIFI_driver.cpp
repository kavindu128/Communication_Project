#include "WIFI_driver.h"
#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID       "ESP32_SENSOR"
#define WIFI_PASSWORD   "12345678"

#define SERVER_IP       "192.168.4.1"
#define SERVER_PORT     5000

WiFiClient client;


bool WIFI_Init()
{
    Serial.println("Starting WiFi Receiver...");

    // Receiver = Station
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    // Wait for WiFi connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected successfully");

    Serial.print("Receiver IP: ");
    Serial.println(WiFi.localIP());


    // Connect to transmitter TCP server
    Serial.println("Connecting to TCP server...");

    while (!client.connected())
    {
        if (client.connect(
                SERVER_IP,
                SERVER_PORT))
        {
            Serial.println("TCP connection successful");
            break;
        }

        Serial.println("TCP connection failed");
        delay(1000);
    }

    return true;
}

bool WIFI_ReceiveData(
    uint8_t *data,
    size_t len
)
{
    if (data == NULL || len == 0)
    {
        return false;
    }

    // Check TCP connection
    if (!client.connected())
    {
        Serial.println("TCP connection lost");

        client.stop();

        if (!client.connect(
                SERVER_IP,
                SERVER_PORT))
        {
            Serial.println("TCP reconnect failed");
            return false;
        }

        Serial.println("TCP reconnected");
    }

    // Wait until complete packet is available
    if (client.available() >= (int)len)
    {
        size_t received = client.read(
            data,
            len
        );

        if (received == len)
        {
            return true;
        }
    }

    return false;
}