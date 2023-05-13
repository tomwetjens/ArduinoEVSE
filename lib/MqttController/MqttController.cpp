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
    if (millis() - lastConnect >= this->settings.reconnectInterval)
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
    if (size > MAX_MSG_LEN)
    {
        size = MAX_MSG_LEN;
    }

    char msg[size];

    if (this->mqttClient->read(msg, size) > 0)
    {
        this->processMessage(msg);
    }
}

void MqttController::processMessage(char *payload)
{
    char *token = strtok(payload, ",");

    Message message = (Message)atoi(token);

    if (message == StartCharging)
    {
        Serial.println("StartCharging message received");
        this->chargeController->startCharging();
        this->sendUpdate();
    }
    else if (message == StopCharging)
    {
        Serial.println("StopCharging message received");
        this->chargeController->stopCharging();
        this->sendUpdate();
    }
    else if (message == SetCurrentLimit)
    {
        Serial.println("SetCurrentLimit message received");
        token = strtok(NULL, ",");
        float currentLimit = atof(token);
        this->loadBalancing->setCurrentLimit(currentLimit);
        this->sendUpdate();
    }
    else if (message == ActualCurrent)
    {
        Serial.println("ActualCurrent message received");
        token = strtok(NULL, ",");
        float actualCurrentL1 = atof(token);
        token = strtok(NULL, ",");
        float actualCurrentL2 = atof(token);
        token = strtok(NULL, ",");
        float actualCurrentL3 = atof(token);
        chargeController->updateActualCurrent({actualCurrentL1, actualCurrentL2, actualCurrentL3});
    }
    else if (message == MainsMeterValues)
    {
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
    }
    else
    {
        Serial.print("Unknown message: ");
        Serial.println(payload);
    }
}

MqttController::MqttController(Client &client,
                               Pilot &pilot,
                               ChargeController &chargeController,
                               LoadBalancing &loadBalancing,
                               MainsMeter &mainsMeter)
{
    this->pilot = &pilot;
    this->chargeController = &chargeController;
    this->loadBalancing = &loadBalancing;
    this->mainsMeter = &mainsMeter;
    this->mqttClient = new MqttClient(client);
    this->lastConnect = 0;
    this->lastUpdateSent = 0;
}

void MqttController::setup(MqttSettings settings)
{
    this->settings = settings;
}

void MqttController::loop()
{
    if (!this->mqttClient->connected())
    {
        this->reconnectAutomatically();
        return;
    }

    int messageSize;
    while (messageSize = this->mqttClient->parseMessage())
    {
        this->onMessage(messageSize);
    }

    this->sendPeriodicUpdate();
}

void MqttController::sendUpdate()
{
    if (!mqttClient->connected())
    {
        return;
    }

    float currentLimit = chargeController->getCurrentLimit();
    float currentLimitFraction = currentLimit - (int)currentLimit;
    int currentLimitDecimals = currentLimitFraction * 10;

    float pilotVoltage = pilot->getVoltage();
    float pilotVoltageFraction = pilotVoltage - (int)pilotVoltage;
    int pilotVoltageDecimals = pilotVoltageFraction * 10;

    float temp = chargeController->getTemp();
    float tempFraction = temp - (int)temp;
    int tempDecimals = tempFraction * 10;

    char msg[100];
    sprintf(msg, "%d,%d,%d.%01d,%d.%01d,%d.%01d",
            chargeController->getState(),
            chargeController->getVehicleState(),
            (int)currentLimit,
            currentLimitDecimals,
            (int)pilotVoltage,
            pilotVoltageDecimals,
            (int)temp,
            tempDecimals);

    Serial.print("Sending message to ");
    Serial.print(settings.outTopic);
    Serial.print(": ");
    Serial.println(msg);

    mqttClient->beginMessage(settings.outTopic);
    mqttClient->print(msg);
    mqttClient->endMessage();

    lastUpdateSent = millis();
}

bool MqttController::isConnected()
{
    return this->mqttClient->connected();
}
