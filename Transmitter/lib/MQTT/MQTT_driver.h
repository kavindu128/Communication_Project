#ifndef MQTT_DRIVER_H
#define MQTT_DRIVER_H

#include "MQTT_hal.h"
#include <PubSubClient.h>
#include "Queues.h"
#include "Sensor_Packet.h"  
#include "WIFI_driver.h"
#include "SIM7600_driver.h"
#include "WIFI_hal.h"
#include "MQTT_hal.h" 

void MQTT_Driver_Init();
bool MQTT_Driver_Connect(); 
void MQTT_Driver_Loop();
bool MQTT_Driver_Publish(const char* topic, const char* payload);
bool MQTT_Driver_IsConnected();

#endif // MQTT_DRIVER_H