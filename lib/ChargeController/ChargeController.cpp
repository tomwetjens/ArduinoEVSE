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

#include "ChargeController.h"
#include "Pilot.h"

#define PIN_AC_RELAY 8

void ChargeController::updateVehicleState()
{
    VehicleState vehicleState = this->pilot->read();

    if (this->vehicleState != vehicleState)
    {
        this->vehicleState = vehicleState;

        Serial.print("Vehicle state: ");
        char vehicleStateText[50];
        vehicleStateToText(vehicleState, vehicleStateText);
        Serial.println(vehicleStateText);

        if (vehicleState != VehicleConnected && vehicleState != VehicleReady && vehicleState != VehicleReadyVentilationRequired)
        {
            if (this->state == Charging)
            {
                this->stopCharging();
            }
        }

        this->applyCurrentLimit();

        if (this->vehicleStateChange)
        {
            this->vehicleStateChange();
        }
    }
}

void ChargeController::detectOverheat()
{
    if (this->tempSensor->read() >= this->settings.overheatTemp)
    {
        if (this->state != Overheat)
        {
            this->stopCharging();

            Serial.println("Overheat! Stopping... You must reset after charger has cooled.");

            this->state = Overheat;
        }
    }
}

ChargeController::ChargeController(IPilot &pilot, ITempSensor &tempSensor)
{
    this->pilot = &pilot;
    this->tempSensor = &tempSensor;
    this->relay = new Relay(PIN_AC_RELAY);
}

void ChargeController::setup(ChargingSettings settings)
{
    relay->setup(3000, 0);

    this->pilot->standby();

    this->settings = settings;
    this->currentLimit = this->settings.maxCurrent;
    this->vehicleState = VehicleNotConnected;
    this->state = Ready;
    this->_actualCurrentUpdated = 0;
}

void ChargeController::loop()
{
    relay->loop();
    this->detectOverheat();
    this->updateVehicleState();
}

void ChargeController::startCharging()
{
    if (this->state == Charging)
    {
        Serial.println("Already charging");
        return;
    }

    if (vehicleState != VehicleConnected && vehicleState != VehicleReady && vehicleState != VehicleReadyVentilationRequired)
    {
        Serial.println("Vehicle not connected");
        return;
    }

    Serial.println("Start charging");

    this->state = Charging;

    this->started = millis();

    this->applyCurrentLimit();

    if (this->stateChange)
    {
        this->stateChange();
    }
}

void ChargeController::stopCharging()
{
    relay->openImmediately(); // immediately, for safety

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

float ChargeController::getTemp()
{
    return this->tempSensor->read();
}

float ChargeController::getCurrentLimit()
{
    return this->currentLimit;
}

void ChargeController::setCurrentLimit(float amps)
{
    if (amps < 0)
    {
        amps = 0;
    }
    else if (amps > this->settings.maxCurrent)
    {
        // May never exceed max current
        amps = this->settings.maxCurrent;
    }

    if (currentLimit != amps)
    {
        this->currentLimit = amps;

        Serial.print("Setting current limit to ");
        Serial.print(amps);
        Serial.println(" A");

        this->applyCurrentLimit();
    }
}

void ChargeController::updateActualCurrent(ActualCurrent actualCurrent)
{
    Serial.print("Updating actual charging current: ");
    Serial.print(actualCurrent.l1);
    Serial.print(" ");
    Serial.print(actualCurrent.l2);
    Serial.print(" ");
    Serial.println(actualCurrent.l3);

    this->_actualCurrent = actualCurrent;
    this->_actualCurrentUpdated = millis();
}

void ChargeController::applyCurrentLimit()
{
    if (this->vehicleState == VehicleConnected || this->vehicleState == VehicleReady || this->vehicleState == VehicleReadyVentilationRequired)
    {
        // Only advertise current limit when there is a vehicle connected

        if (currentLimit >= MIN_CURRENT)
        {
            pilot->currentLimit(currentLimit);

            if (this->state == Charging && (vehicleState == VehicleReady || vehicleState == VehicleReadyVentilationRequired))
            {
                // Close relay again if it was temporarily open
                relay->close();
            }
            else
            {
                // Make sure it is open if not charging or not ready
                relay->open();
            }
        }
        else
        {
            // We need to advertise at least 6A to the EV, so for anything less just we go to standby
            relay->open();
            pilot->standby();
        }
    }
    else
    {
        // Switch pilot to standby as soon as vehicle is no longer connected
        relay->open();
        pilot->standby();
    }
}

unsigned long ChargeController::getElapsedTime()
{
    return millis() - this->started;
}

void ChargeController::onVehicleStateChange(ChargeControllerEventHandler handler)
{
    this->vehicleStateChange = handler;
}

void ChargeController::onStateChange(ChargeControllerEventHandler handler)
{
    this->stateChange = handler;
}
