#include <Arduino.h>
#include "ChargeController.h"
#include "Pilot.h"

#define PIN_AC_RELAY 8

#define PIN_CT_IN PIN_A0

#define USING_TIMERB true

#define EV_STATE_DEBOUNCE_MILLIS 500

Pilot pilot;
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

                this->vehicleStateChange();
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

    pilot.standby();

    this->maxCurrent = 16;
    this->desiredCurrent = 16;
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

    pilot.currentLimit(desiredCurrent);
    this->closeRelay();

    this->state = Charging;

    this->startMillis = millis();

    this->stateChange();
}

void ChargeController::stopCharging()
{
    this->openRelay();
    pilot.standby();

    if (this->state != Charging)
    {
        Serial.println("Not charging");
        return;
    }

    Serial.println("Stop charging");

    this->state = Ready;
    this->stateChange();
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

int ChargeController::getDesiredCurrent()
{
    return this->desiredCurrent;
}

float ChargeController::getActualCurrent()
{
    return 0; // TODO
}

void ChargeController::onVehicleStateChange(EventHandler handler)
{
    this->vehicleStateChange = handler;
}

void ChargeController::onStateChange(EventHandler handler)
{
    this->stateChange = handler;
}
