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
#include <math.h>

#include "Pilot.h"

#define PILOT_FREQUENCY 1000 // 1 kHz
#define PIN_PILOT_PWM 10

#define PIN_PILOT_IN PIN_A1

#ifdef ARDUINO_AVR_UNO_WIFI_REV2
#include "megaAVR_PWM.h"

#define USING_TIMERB true
megaAVR_PWM *PWM_Instance = new megaAVR_PWM(PIN_PILOT_PWM, PILOT_FREQUENCY, 0);
#endif

int analogReadMax(uint8_t pinNumber, uint8_t count)
{
    long result = 0;
    for (int i = 0; i < count; i++)
    {
        result = fmax(result, analogRead(pinNumber));
    }
    return result;
}

void Pilot::standby()
{
    Serial.println("Setting pilot standby");
    digitalWrite(PIN_PILOT_PWM, 1);
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

#ifdef ARDUINO_AVR_UNO_WIFI_REV2
    PWM_Instance->setPWM(PIN_PILOT_PWM, PILOT_FREQUENCY, dutyCycle);
#endif
}

float Pilot::getVoltage()
{
    return this->voltage;
}

float Pilot::readPin()
{
    int pinValue = analogReadMax(PIN_PILOT_IN, 100);                                                                           // 0-1024
    float pinVoltage = (pinValue / 1023.0) * 5.0;                                                                             // 0-5V
    this->voltage = pinVoltage * 5.44 - 12.6; // magic values based on the hardware
    return this->voltage;
}

VehicleState Pilot::read()
{
    float voltage = floor(this->readPin());

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
        strcpy(buffer, "Not connected");
        break;
    case VehicleConnected:
        strcpy(buffer, "Connected, not ready");
        break;
    case VehicleReady:
        strcpy(buffer, "Ready");
        break;
    case VehicleReadyVentilationRequired:
        strcpy(buffer, "Ready, ventilation required");
        break;
    case VehicleNoPower:
        strcpy(buffer, "No power");
        break;
    case VehicleError:
        strcpy(buffer, "Error");
        break;
    default:
        strcpy(buffer, "Unknown");
        break;
    }
}