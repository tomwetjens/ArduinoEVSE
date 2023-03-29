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
    VehicleState vehicleState = pilot.read();

    if (this->vehicleState != vehicleState)
    {
        this->vehicleState = vehicleState;

        Serial.print("Vehicle state: ");
        Serial.println(vehicleStateToText(vehicleState));

        if (vehicleState == VehicleConnected || vehicleState == VehicleReady)
        {
            // Switch pilot from standby to advertising the current limit as soon as vehicle is connected/ready
            pilot.currentLimit(this->currentLimit);
        }
        else if (vehicleState != VehicleConnected && vehicleState != VehicleReady)
        {
            // Switch pilot to standby as soon as vehicle is disconnected
            pilot.standby();
        }

        if (this->vehicleStateChange)
        {
            this->vehicleStateChange();
        }
    }
}

void ChargeController::fallbackCurrentIfNeeded()
{
    // When timeout is configured (indicating that fallback is enabled)
    if (this->settings.loadBalancing.currentLimitTimeout > 0)
    {
        // When current limit is outdated (timeout exceeded)
        if (millis() - currentLimitLastUpdated >= this->settings.loadBalancing.currentLimitTimeout)
        {
            // For safety reasons, we have to fall back to a safe charging current

            // When not already at a safe current
            if (currentLimit > this->settings.loadBalancing.fallbackCurrent)
            {
                Serial.println("Current limit timeout. Falling back to safe charging current");
                setCurrentLimit(this->settings.loadBalancing.fallbackCurrent);
            }
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

void ChargeController::setup(ChargingSettings settings)
{
    pinMode(PIN_AC_RELAY, OUTPUT);
    openRelay();

    this->pilot = Pilot();
    this->pilot.standby();

    this->settings = settings;
    this->currentLimit = this->settings.maxCurrent;
    this->currentLimitLastUpdated = millis();
    this->vehicleState = VehicleNotConnected;
    this->state = Ready;
}

void ChargeController::loop()
{
    this->updateVehicleState();

    if (this->vehicleState != VehicleReady)
    {
        if (this->state == Charging)
        {
            this->stopCharging();
        }
    }

    this->fallbackCurrentIfNeeded();
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
    this->currentLimitLastUpdated = millis();

    Serial.print("Setting current limit to ");
    Serial.print(amps);
    Serial.println(" A");

    if (this->vehicleState == VehicleConnected || this->vehicleState == VehicleReady)
    {
        pilot.currentLimit(currentLimit);

        if (currentLimit < MIN_CURRENT)
        {
            // We need to advertise at least 6A to the EV, so for anything less just open relay (temporarily)
            this->openRelay();
        }
        else
        {
            // Ensure relay is closed
            this->closeRelay();
        }
    }
}

unsigned long ChargeController::getElapsedTime()
{
    return millis() - this->started;
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
