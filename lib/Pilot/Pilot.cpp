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
#include "megaAVR_PWM.h"

#include "Pilot.h"

#define PILOT_FREQUENCY 1000 // 1 kHz
#define PILOT_STANDBY_DUTY_CYCLE 99
#define PIN_PILOT_PWM 10

#define PIN_PILOT_IN_MIN_VOLTAGE 2.55 // @ 0V pilot, measured
#define PIN_PILOT_IN_MAX_VOLTAGE 4.47 // @ 12V pilot, measured
#define PIN_PILOT_IN PIN_A1

#define USING_TIMERB true
megaAVR_PWM *PWM_Instance = new megaAVR_PWM(PIN_PILOT_PWM, PILOT_FREQUENCY, 0);

int analogReadMax(pin_size_t pinNumber, int count)
{
    long result = 0;
    for (int i = 0; i < count; i++)
    {
        result = max(result, analogRead(pinNumber));
    }
    return result;
}

void Pilot::standby()
{
    Serial.println("Setting pilot standby");
    digitalWrite(PIN_PILOT_PWM, HIGH);
}

void Pilot::currentLimit(float amps)
{
    if (amps < MIN_CURRENT)
        amps = MIN_CURRENT;
    else if (amps > MAX_CURRENT)
        amps = MAX_CURRENT;

    int dutyCycle;
    if (amps <= 51)
    {
        // 6A - 51A
        dutyCycle = amps / 0.6;
    }
    else
    {
        // 51A - 80A
        dutyCycle = (amps / 2.5) + 64;
    }

    Serial.print("Setting pilot duty cycle: ");
    Serial.println(dutyCycle);
    PWM_Instance->setPWM(PIN_PILOT_PWM, PILOT_FREQUENCY, dutyCycle);
}

float Pilot::getVoltage()
{
    return this->voltage;
}

float Pilot::readPin()
{
    int pinValue = analogReadMax(PIN_PILOT_IN, 10); // 0-1024
    float pinVoltage = (pinValue / 1023.0) * 5.0;   // 0-5V
    this->voltage  = ((pinVoltage - PIN_PILOT_IN_MIN_VOLTAGE) / (PIN_PILOT_IN_MAX_VOLTAGE - PIN_PILOT_IN_MIN_VOLTAGE)) * 12.0; // 0-12V
    return this->voltage;
}

VehicleState Pilot::read()
{
    float voltage = this->readPin();

    if (voltage >= 11) // 12V +/-1V
    {
        return VehicleNotConnected;
    }
    else if (voltage >= 8) // 9V +/-1V
    {
        return VehicleConnected;
    }
    else if (voltage >= 5) // 6V +/-1V
    {
        return VehicleReady;
    }
    else if (voltage >= 2) // 3V +/-1V
    {
        return VehicleReadyVentilationRequired;
    }
    else if (voltage >= 0) // 0V
    {
        return VehicleNoPower;
    }
    else
    {
        return VehicleError; // -12V
    }
}

void vehicleStateToText(VehicleState vehicleState, char *buffer)
{
    switch (vehicleState)
    {
    case VehicleNotConnected:
        buffer = "Not connected";
    case VehicleConnected:
        buffer = "Connected, not ready";
    case VehicleReady:
        buffer = "Ready";
    case VehicleReadyVentilationRequired:
        buffer = "Ready, ventilation required";
    case VehicleNoPower:
        buffer = "No power";
    case VehicleError:
        buffer = "Error";
    default:
        buffer = "Unknown";
    }
}