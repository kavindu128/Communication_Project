#ifndef MQTT_DRIVER_H
#define MQTT_DRIVER_H

#include "MQTT_hal.h"
#include <PubSubClient.h>
#include "Queues.h"
#include "Sensor_Packet.h"  
#include "WIFI_driver.h"
#include "SIM7600_driver.h"
#include "WIFI_hal.h"
#include "SIM7600_app.h"

void MQTT_Driver_Init();                                          // broker settings only, no network needed
void MQTT_Driver_BindClient(Client* client, uint16_t timeoutSec); // attach to WiFi or 4G TCP stream
bool MQTT_Driver_Connect(); 
void MQTT_Driver_Disconnect();                                    // clean MQTT DISCONNECT + close socket
void MQTT_Driver_Loop();
bool MQTT_Driver_Publish(const char* topic, const char* payload);
bool MQTT_Driver_IsConnected();

#endif // MQTT_DRIVER_H
