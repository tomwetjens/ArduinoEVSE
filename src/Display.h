#ifndef DISPLAY_H_ _
#define DISPLAY_H_ _

#include "ChargeController.h"
#include "NetworkManager.h"
#include "MqttController.h"

class Display
{
private:
    ChargeController *chargeController;
    NetworkManager *networkManager;
    MqttController *mqttController;
    State lastUpdateState;
    unsigned long lastUpdateMillis;
    void printStatus(String status);
    void printActualCurrent(float amps);
    void printCurrentLimit(float amps);
    void printElapsedTime();

public:
    Display(ChargeController &chargeController,
            NetworkManager &networkManager,
            MqttController &mqttController);
    void setup();
    void update();
};

#endif