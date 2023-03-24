#include <Arduino.h>

#include <WiFiNINA.h>
#include <WiFiClient.h>

#include <ArduinoMqttClient.h>

#include "MqttController.h"

const char host[] = "192.168.1.44";
const int port = 1883;
const char inTopic[] = "ArduinoEVSE/in";
const char outTopic[] = "ArduinoEVSE/out";
const unsigned long reconnectInterval = 5000;
const unsigned long updateInterval = 5000;

WiFiClient wifiClient;

MqttClient mqttClient(wifiClient);

unsigned long lastConnectMillis = 0;
unsigned long lastUpdateSentMillis = 0;

#define MAX_MSG_LEN 50
char msg[MAX_MSG_LEN];

void MqttController::connect()
{
    lastConnectMillis = millis();

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.print(host);
    Serial.print(":");
    Serial.println(port);

    mqttClient.stop();
    if (!mqttClient.connect(host, port))
    {
        Serial.print("MQTT connection failed! Error code: ");
        Serial.println(mqttClient.connectError());
    }

    Serial.println("Connected to the MQTT broker");

    Serial.print("Subscribing to topic: ");
    Serial.println(inTopic);
    mqttClient.subscribe(inTopic);

    this->sendUpdate();
}

void MqttController::reconnectAutomatically()
{
    if (WiFi.status() == WL_CONNECTED && millis() - lastConnectMillis >= reconnectInterval)
    {
        this->connect();
    }
}

void MqttController::sendPeriodicUpdate()
{
    if (millis() - lastUpdateSentMillis >= updateInterval)
    {
        this->sendUpdate();
    }
}

void MqttController::onMessage(int size)
{
    memset(msg, '\0', MAX_MSG_LEN);
    if (size > MAX_MSG_LEN)
        size = MAX_MSG_LEN;
    if (mqttClient.read(msg, size) > 0)
    {
        this->processMessage(msg);
    }
}

void MqttController::processMessage(char *msg)
{
    Serial.print("Message: ");
    Serial.println(msg);

    char *token = strtok(msg, ",");

    Command cmd = (Command)atoi(token);

    switch (cmd)
    {
    case StartChargingSession:
        Serial.println("StartChargingSession command received");
        this->chargeController->startCharging();
        this->sendUpdate();
        break;

    case StopChargingSession:
        Serial.println("StopChargingSession command received");
        this->chargeController->stopCharging();
        this->sendUpdate();
        break;

    case SetCurrentLimit:
        Serial.println("SetCurrentLimit command received");

        token = strtok(NULL, ",");
        float amps = atof(token);

        this->chargeController->setCurrentLimit(amps);
        this->sendUpdate();
        break;

    default:
        Serial.print("Unknown message:");
        Serial.println(msg);
    }
}

MqttController::MqttController(ChargeController &chargeController)
{
    this->chargeController = &chargeController;
}

void MqttController::setup()
{
}

void MqttController::loop()
{
    if (!mqttClient.connected() || WiFi.status() != WL_CONNECTED)
    {
        this->reconnectAutomatically();
        return;
    }

    int messageSize;
    while (messageSize = mqttClient.parseMessage())
    {
        Serial.print("Received message of size: ");
        Serial.println(messageSize);
        this->onMessage(messageSize);
    }

    this->sendPeriodicUpdate();
}

void MqttController::sendUpdate()
{
    if (!mqttClient.connected())
    {
        return;
    }

    float currentLimit = this->chargeController->getCurrentLimit();
    float currentLimitFraction = currentLimit - (int)currentLimit;
    int currentLimitDecimals = currentLimitFraction * 10;

    Pilot* pilot = this->chargeController->getPilot();
    float pilotVoltage = pilot->getLastPilotVoltage();
    float pilotVoltageFraction = pilotVoltage - (int)pilotVoltage;
    int pilotVoltageDecimals = pilotVoltageFraction * 10;

    char msg[100];
    sprintf(msg, "%d,%d,%d.%01d,%d.%02d",
            this->chargeController->getState(),
            this->chargeController->getVehicleState(),
            (int)currentLimit,
            currentLimitDecimals,
            (int)pilotVoltage,
            pilotVoltageDecimals);

    Serial.print("Sending message to ");
    Serial.print(outTopic);
    Serial.print(": ");
    Serial.println(msg);

    mqttClient.beginMessage(outTopic);
    mqttClient.print(msg);
    mqttClient.endMessage();

    lastUpdateSentMillis = millis();
}

bool MqttController::isConnected()
{
    return mqttClient.connected();
}
