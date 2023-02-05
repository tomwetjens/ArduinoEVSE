#ifndef MQTTCONTROLLER_H_  _
#define MQTTCONTROLLER_H_  _

#include <WiFiClient.h>
#include <ArduinoMqttClient.h>

#include "ChargeController.h"

enum Command
{
    StopChargingSession = 0,
    StartChargingSession = 1
};

class MqttController
{
private:
    ChargeController* chargeController;
    void connect();
    void reconnectAutomatically();
    void sendPeriodicUpdate();
    void onMessage(int size);
    void processMessage(char *msg);

public:
    MqttController(ChargeController& chargeController);
    void setup();
    void loop();
    void sendUpdate();
};

#endif