/*
 * This file is part of the ArduinoEVSE (https://github.com/tomwetjens/ArduinoEVSE).
 * Copyright (c) 2023 Tom Wetjens.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MQTTCONTROLLER_H_
#define MQTTCONTROLLER_H_ _

#include <ArduinoMqttClient.h>

#include <NetworkManager.h>
#include <ChargeController.h>
#include <LoadBalancing.h>
#include <MainsMeter.h>

enum Message
{
    StopCharging = 0,
    StartCharging = 1,
    SetCurrentLimit = 2,
    ActualCurrent = 3,
    MainsMeterValues = 4,
};

struct MqttSettings
{
    char host[254];
    uint16_t port = 1883;
    char username[100];
    char password[100];
    char inTopic[100] = "ArduinoEVSE/in";
    char outTopic[100] = "ArduinoEVSE/out";
    uint16_t reconnectInterval = 5000;
    uint16_t updateInterval = 5000;
};

class MqttController
{
private:
    NetworkManager *networkManager;
    Pilot *pilot;
    ChargeController *chargeController;
    LoadBalancing *loadBalancing;
    MainsMeter *mainsMeter;
    MqttSettings settings;
    MqttClient *mqttClient;
    unsigned long lastConnect = 0;
    unsigned long lastUpdateSent = 0;
    void connect();
    void reconnectAutomatically();
    void sendPeriodicUpdate();
    void onMessage(int size);
    void processMessage(char *msg);

public:
    MqttController(NetworkManager &networkManager, Pilot &pilot, ChargeController &chargeController, LoadBalancing &loadBalancing, MainsMeter &mainsMeter);

    void setup(MqttSettings settings);
    void loop();

    void sendUpdate();

    bool isConnected();
};

#endif // MQTTCONTROLLER_H_