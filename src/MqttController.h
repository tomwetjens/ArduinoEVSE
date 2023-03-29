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

#ifndef MQTTCONTROLLER_H_ _
#define MQTTCONTROLLER_H_ _

#include <WiFiClient.h>
#include <ArduinoMqttClient.h>

#include "ChargeController.h"

#include "arduino_secrets.h"

enum Command
{
    StopChargingSession = 0,
    StartChargingSession = 1,
    SetCurrentLimit = 2,
};

struct MqttSettings
{
    char host[253] = MQTT_HOST;
    int port = 1883;
    char inTopic[100] = "ArduinoEVSE/in";
    char outTopic[100] = "ArduinoEVSE/out";
    unsigned long reconnectInterval = 5000;
    unsigned long updateInterval = 5000;
};

class MqttController
{
private:
    ChargeController *chargeController;
    MqttSettings settings;
    WiFiClient *wifiClient;
    MqttClient *mqttClient;
    unsigned long lastConnect = 0;
    unsigned long lastUpdateSent = 0;
    void connect();
    void reconnectAutomatically();
    void sendPeriodicUpdate();
    void onMessage(int size);
    void processMessage(char *msg);

public:
    MqttController(ChargeController &chargeController);

    void setup(MqttSettings settings);
    void loop();

    void sendUpdate();

    bool isConnected();
};

#endif // MQTTCONTROLLER_H_