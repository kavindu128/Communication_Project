#include "MQTT_driver.h"

//const char* BROKER = "mqtt.thingsboard.cloud";
const char* BROKER = "54.159.242.170";
const int PORT = 1883;
const char* CLIENT_ID = "001";
const char* USERNAME  = "Test01";
const char* PASSWORD  = "test1234";

static PubSubClient mqttClient;
static bool clientBound = false;

void MQTT_Driver_Init() {
    // No client is bound here: the network may not exist yet.
    // MQTT_app binds WiFi or 4G when one of them is really up.
    mqttClient.setServer(BROKER, PORT);
    mqttClient.setKeepAlive(60);
    mqttClient.setBufferSize(512);
}

void MQTT_Driver_BindClient(Client* client, uint16_t timeoutSec) {
    if (client == nullptr) {
        clientBound = false;
        return;
    }
    mqttClient.setClient(*client);
    mqttClient.setSocketTimeout(timeoutSec);   // short for WiFi, long for cellular latency
    clientBound = true;
}

bool MQTT_Driver_Connect() {
    if (!clientBound) return false;
    if (mqttClient.connected()) return true;

    Serial.print("Connecting to MQTT broker...");
    if (mqttClient.connect(CLIENT_ID, USERNAME, PASSWORD)) {
        Serial.println(" connected!");
        return true;
    } else {
        Serial.print(" failed, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

void MQTT_Driver_Disconnect() {
    if (!clientBound) return;
    mqttClient.disconnect();
}

void MQTT_Driver_Loop() {
    if (!clientBound) return;
    mqttClient.loop();
}

bool MQTT_Driver_Publish(const char* topic, const char* payload) {
    if (!clientBound) return false;
    return mqttClient.publish(topic, payload);
}

bool MQTT_Driver_IsConnected() {
    if (!clientBound) return false;
    return mqttClient.connected();
}
