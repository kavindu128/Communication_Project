#include "MQTT_driver.h"
                       

//const char* BROKER = "mqtt.thingsboard.cloud";
const char* BROKER = "54.159.242.170";
const int PORT = 1883;
const char* CLIENT_ID = "001";
const char* USERNAME  = "Test01";
const char* PASSWORD  = "test1234";

static PubSubClient mqttClient;

void MQTT_Driver_Init() {
   Client* activeClient = MQTT_HAL_GetActiveClient();
    if (activeClient != nullptr) {
        mqttClient.setClient(*activeClient);
    }
    mqttClient.setServer(BROKER, PORT);
    
    // INCREASE TIMEOUTS FOR CELLULAR LATENCY
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(30);
}

bool MQTT_Driver_Connect() {
    // 1. Fetch whichever network (WiFi or SIM) is currently active
    Client* activeClient = MQTT_HAL_GetActiveClient();
    
    if (activeClient == nullptr) {
        return false; // Neither network is connected
    }
    
    // 2. Bind the PubSubClient to the active hardware
    mqttClient.setClient(*activeClient);

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

void MQTT_Driver_Loop() {
    mqttClient.loop();
}

bool MQTT_Driver_Publish(const char* topic, const char* payload) {
    return mqttClient.publish(topic, payload);
}

bool MQTT_Driver_IsConnected() {
    return mqttClient.connected();
}