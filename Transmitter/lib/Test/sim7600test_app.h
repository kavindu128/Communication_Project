#ifndef SIM7600_TEST_APP_H
#define SIM7600_TEST_APP_H

#include <Arduino.h>
#include "SIM7600_app.h"

extern TaskHandle_t SIM7600TestHandle;

// Test task for the SIM7600 state machine.
// Type in the serial monitor:  O = ON    X = OFF    S = state
void SIM7600_Test_app(void * pvParameters);

#endif