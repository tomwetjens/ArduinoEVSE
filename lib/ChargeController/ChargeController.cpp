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

        if (this->vehicleState != VehicleReady)
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


void ChargeController::closeRelay()
{
    digitalWrite(PIN_AC_RELAY, HIGH);
}

void ChargeController::openRelay()
{
    digitalWrite(PIN_AC_RELAY, LOW);
}

ChargeController::ChargeController(Pilot &pilot, TempSensor &tempSensor)
{
    this->pilot = &pilot;
    this->tempSensor = &tempSensor;
}

void ChargeController::setup(ChargingSettings settings)
{
    pinMode(PIN_AC_RELAY, OUTPUT);
    this->openRelay();

    this->pilot->standby();

    this->settings = settings;
    this->currentLimit = this->settings.maxCurrent;
    this->vehicleState = VehicleNotConnected;
    this->state = Ready;
}

void ChargeController::loop()
{
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

    if (this->vehicleState != VehicleReady)
    {
        Serial.println("Vehicle not ready");
        return;
    }

    Serial.println("Start charging");
    this->closeRelay();

    this->state = Charging;

    this->started = millis();

    if (this->stateChange)
    {
        this->stateChange();
    }
}

void ChargeController::stopCharging()
{
    // Ensure relay is open regardless of administrative state
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

float ChargeController::getTemp()
{
    return this->tempSensor->getLastRead();
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

    this->currentLimit = amps;

    Serial.print("Setting current limit to ");
    Serial.print(amps);
    Serial.println(" A");

    this->applyCurrentLimit();
}

void ChargeController::updateActualCurrent(float amps)
{
    this->_actualCurrent = amps;
    this->_actualCurrentUpdated = millis();
}

void ChargeController::applyCurrentLimit()
{
    // Only advertise current limit when there is a vehicle connected
    if (this->vehicleState == VehicleConnected || this->vehicleState == VehicleReady)
    {
        if (currentLimit < MIN_CURRENT)
        {
            // We need to advertise at least 6A to the EV, so for anything less just we go to standby
            this->openRelay();
            this->pilot->standby();
        }
        else
        {
            // Switch pilot from standby to advertising the current limit as soon as vehicle is connected/ready
            this->pilot->currentLimit(currentLimit);

            if (this->state == Charging)
            {
                // Close relay again if it was temporarily open
                this->closeRelay();
            }
        }
    }
    else if (vehicleState != VehicleConnected && vehicleState != VehicleReady)
    {
        // Switch pilot to standby as soon as vehicle is disconnected or no longer ready
        this->pilot->standby();
        // Ensure relay is open
        this->openRelay();
    }
}

unsigned long ChargeController::getElapsedTime()
{
    return millis() - this->started;
}

Pilot *ChargeController::getPilot()
{
    return this->pilot;
}

void ChargeController::onVehicleStateChange(ChargeControllerEventHandler handler)
{
    this->vehicleStateChange = handler;
}

void ChargeController::onStateChange(ChargeControllerEventHandler handler)
{
    this->stateChange = handler;
}
