#include <Arduino.h>
#include "ChargeController.h"
#include "Pilot.h"

#define PIN_AC_RELAY 8

#define EV_STATE_DEBOUNCE_MILLIS 500

unsigned long lastReadVehicleStateMillis = 0;
VehicleState lastReadVehicleState = EV_NotConnected;

String getVehicleStateText(VehicleState vehicleState)
{
    switch (vehicleState)
    {
    case EV_NotConnected:
        return "Not connected";
    case EV_Connected:
        return "Connected, not ready";
    case EV_Ready:
        return "Ready";
    case EV_ReadyVentilationRequired:
        return "Ready, ventilation required";
    case EV_NoPower:
        return "No power";
    case EV_Error:
        return "Error";
    default:
        return "Unknown";
    }
}

void ChargeController::updateVehicleState()
{
    VehicleState vehicleState = pilot.read();

    if (lastReadVehicleState == vehicleState)
    {
        if (millis() - lastReadVehicleStateMillis >= EV_STATE_DEBOUNCE_MILLIS)
        {
            if (this->vehicleState != vehicleState)
            {
                this->vehicleState = vehicleState;

                Serial.print("Vehicle state: ");
                Serial.println(getVehicleStateText(vehicleState));

                if (vehicleState == EV_Connected || vehicleState == EV_Ready)
                {
                    pilot.currentLimit(this->currentLimit);
                }
                else if (vehicleState != EV_Connected && vehicleState != EV_Ready)
                {
                    pilot.standby();
                }

                if (this->vehicleStateChange)
                {
                    this->vehicleStateChange();
                }
            }
        }
    }
    else
    {
        lastReadVehicleState = vehicleState;
        lastReadVehicleStateMillis = millis();
    }
}

void ChargeController::closeRelay()
{
    digitalWrite(PIN_AC_RELAY, HIGH);
}

void ChargeController::openRelay()
{
    digitalWrite(PIN_AC_RELAY, LOW);
}

void ChargeController::setup()
{
    pinMode(PIN_AC_RELAY, OUTPUT);
    openRelay();

    this->pilot = Pilot();
    this->pilot.standby();

    this->maxCurrent = 16;
    this->currentLimit = this->maxCurrent;
    this->vehicleState = EV_NotConnected;
    this->state = Ready;
}

void ChargeController::update()
{
    this->updateVehicleState();

    if (this->vehicleState != EV_Ready)
    {
        if (this->state == Charging)
        {
            this->stopCharging();
        }
    }
}

void ChargeController::startCharging()
{
    if (this->state == Charging)
    {
        Serial.println("Already charging");
        return;
    }

    if (this->vehicleState != EV_Ready)
    {
        Serial.println("Vehicle not ready");
        return;
    }

    Serial.println("Start charging");

    pilot.currentLimit(this->currentLimit);
    this->closeRelay();

    this->state = Charging;

    this->startMillis = millis();

    if (this->stateChange)
    {
        this->stateChange();
    }
}

void ChargeController::stopCharging()
{
    this->openRelay();

    if (this->state != Charging)
    {
        Serial.println("Not charging");
        return;
    }

    Serial.println("Stop charging");

    this->state = Ready;

    if (this->stateChange)
    {
        this->stateChange();
    }
}

State ChargeController::getState()
{
    return this->state;
}

VehicleState ChargeController::getVehicleState()
{
    return this->vehicleState;
}

unsigned long ChargeController::getElapsedTime()
{
    return millis() - this->startMillis;
}

float ChargeController::getCurrentLimit()
{
    return this->currentLimit;
}

void ChargeController::setCurrentLimit(float amps)
{
    this->currentLimit = amps;

    Serial.print("Setting current limit to ");
    Serial.print(amps);
    Serial.println(" A");

    if (this->vehicleState == EV_Connected || this->vehicleState == EV_Ready)
    {
        pilot.currentLimit(currentLimit);
    }
}

Pilot *ChargeController::getPilot()
{
    return &this->pilot;
}

void ChargeController::onVehicleStateChange(EventHandler handler)
{
    this->vehicleStateChange = handler;
}

void ChargeController::onStateChange(EventHandler handler)
{
    this->stateChange = handler;
}
