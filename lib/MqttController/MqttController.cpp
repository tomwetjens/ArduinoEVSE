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

#include <Arduino.h>

#include <WiFiNINA.h>
#include <WiFiClient.h>

#include <ArduinoMqttClient.h>
#include <MqttController.h>

#define MAX_MSG_LEN 50

void MqttController::connect()
{
    this->lastConnect = millis();

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.print(this->settings.host);
    Serial.print(":");
    Serial.println(this->settings.port);

    this->mqttClient->stop(); // ensure socket is closed in case of reconnect
    if (!this->mqttClient->connect(this->settings.host, this->settings.port))
    {
        Serial.print("MQTT connection failed! Error code: ");
        Serial.println(this->mqttClient->connectError());
    }

    Serial.println("Connected to the MQTT broker");

    Serial.print("Subscribing to topic: ");
    Serial.println(this->settings.inTopic);
    this->mqttClient->subscribe(this->settings.inTopic);

    this->sendUpdate();
}

void MqttController::reconnectAutomatically()
{
    if (WiFi.status() == WL_CONNECTED && millis() - lastConnect >= this->settings.reconnectInterval)
    {
        this->connect();
    }
}

void MqttController::sendPeriodicUpdate()
{
    if (millis() - lastUpdateSent >= this->settings.updateInterval)
    {
        this->sendUpdate();
    }
}

void MqttController::onMessage(int size)
{
    char msg[MAX_MSG_LEN];
    memset(msg, '\0', MAX_MSG_LEN);

    if (size > MAX_MSG_LEN)
    {
        size = MAX_MSG_LEN;
    }

    if (this->mqttClient->read(msg, size) > 0)
    {
        this->processMessage(msg);
    }
}

void MqttController::processMessage(char *payload)
{
    Serial.print("Message: ");
    Serial.println(payload);

    char *token = strtok(payload, ",");

    Message message = (Message)atoi(token);

    switch (message)
    {
    case StartChargingSession:
        Serial.println("StartChargingSession message received");
        this->chargeController->startCharging();
        this->sendUpdate();
        break;

    case StopChargingSession:
        Serial.println("StopChargingSession message received");
        this->chargeController->stopCharging();
        this->sendUpdate();
        break;

    case SetCurrentLimit:
        Serial.println("SetCurrentLimit message received");
        token = strtok(NULL, ",");
        float currentLimit = atof(token);
        this->loadBalancing->setCurrentLimit(currentLimit);
        this->sendUpdate();
        break;

    case ActualCurrent:
        Serial.println("ActualCurrent message received");
        token = strtok(NULL, ",");
        float actualCurrent = atof(token);
        chargeController->updateActualCurrent(actualCurrent);
        break;

    case MainsMeterValues:
        Serial.println("MainsMeterValues message received");
        token = strtok(NULL, ",");
        float importCurrentL1 = atof(token);
        token = strtok(NULL, ",");
        float importCurrentL2 = atof(token);
        token = strtok(NULL, ",");
        float importCurrentL3 = atof(token);
        token = strtok(NULL, ",");
        float exportCurrentL1 = atof(token);
        token = strtok(NULL, ",");
        float exportCurrentL2 = atof(token);
        token = strtok(NULL, ",");
        float exportCurrentL3 = atof(token);
        mainsMeter->updateValues({importCurrentL1, importCurrentL2, importCurrentL3}, {exportCurrentL1, exportCurrentL2, exportCurrentL3});
        break;

    default:
        Serial.print("Unknown message:");
        Serial.println(payload);
    }
}

MqttController::MqttController(ChargeController &chargeController, LoadBalancing &loadBalancing, MainsMeter &mainsMeter)
{
    this->chargeController = &chargeController;
    this->loadBalancing = &loadBalancing;
    this->mainsMeter = &mainsMeter;
    this->wifiClient = new WiFiClient();
    this->mqttClient = new MqttClient(wifiClient);
    this->lastConnect = 0;
    this->lastUpdateSent = 0;
}

void MqttController::setup(MqttSettings settings)
{
    this->settings = settings;
}

void MqttController::loop()
{
    if (!this->mqttClient->connected() || WiFi.status() != WL_CONNECTED)
    {
        this->reconnectAutomatically();
        return;
    }

    int messageSize;
    while (messageSize = this->mqttClient->parseMessage())
    {
        Serial.print("Received message of size: ");
        Serial.println(messageSize);
        this->onMessage(messageSize);
    }

    this->sendPeriodicUpdate();
}

void MqttController::sendUpdate()
{
    if (!this->mqttClient->connected())
    {
        return;
    }

    float currentLimit = this->chargeController->getCurrentLimit();
    float currentLimitFraction = currentLimit - (int)currentLimit;
    int currentLimitDecimals = currentLimitFraction * 10;

    Pilot *pilot = this->chargeController->getPilot();
    float pilotVoltage = pilot->getVoltage();
    float pilotVoltageFraction = pilotVoltage - (int)pilotVoltage;
    int pilotVoltageDecimals = pilotVoltageFraction * 10;

    float temp = this->chargeController->getTemp();
    float tempFraction = temp - (int)temp;
    int tempDecimals = tempFraction * 10;

    char msg[100];
    sprintf(msg, "%d,%d,%d.%01d,%d.%01d,%d.%01d",
            this->chargeController->getState(),
            this->chargeController->getVehicleState(),
            (int)currentLimit,
            currentLimitDecimals,
            (int)pilotVoltage,
            pilotVoltageDecimals,
            (int)temp,
            tempDecimals);

    Serial.print("Sending message to ");
    Serial.print(this->settings.outTopic);
    Serial.print(": ");
    Serial.println(msg);

    this->mqttClient->beginMessage(this->settings.outTopic);
    this->mqttClient->print(msg);
    this->mqttClient->endMessage();

    this->lastUpdateSent = millis();
}

bool MqttController::isConnected()
{
    return this->mqttClient->connected();
}
