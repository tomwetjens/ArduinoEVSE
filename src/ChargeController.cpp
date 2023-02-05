#include <Arduino.h>

#include "ChargeController.h"

#define PIN_PILOT_PWM digitalPinToPort(10)
#define PIN_PILOT_IN PIN_A0
#define PIN_AC_RELAY 8
#define PIN_CT_IN PIN_A1

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

VehicleState readVehicleState()
{
    float pinVoltage = (analogRead(PIN_PILOT_IN) / 1023.0) * 5;
    float pilotVoltage = (pinVoltage / 5) * 12;

    if (pilotVoltage >= 11)
    {
        return EV_NotConnected;
    }
    else if (pilotVoltage >= 8)
    {
        return EV_Connected;
    }
    else if (pilotVoltage >= 5)
    {
        return EV_Ready;
    }
    else if (pilotVoltage >= 2)
    {
        return EV_ReadyVentilationRequired;
    }
    else if (pilotVoltage >= 0)
    {
        return EV_NoPower;
    }
    else
    {
        return EV_Error;
    }
}

void ChargeController::updateVehicleState()
{
    VehicleState vehicleState = readVehicleState();

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

void pilotStandby()
{
    analogWrite(PIN_PILOT_PWM, 127); // 50% duty cycle @ ~1 kHz
}

void pilotCurrentLimit(int amps)
{
    analogWrite(PIN_PILOT_PWM, (amps / 0.6) * 1023);
}

void ChargeController::setup()
{
    pinMode(PIN_AC_RELAY, OUTPUT);
    openRelay();

    pilotStandby();
    pinMode(PIN_PILOT_IN, INPUT);

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

    pilotCurrentLimit(desiredCurrent);
    this->closeRelay();

    this->state = Charging;
    this->stateChange();
}

void ChargeController::stopCharging()
{
    this->openRelay();
    pilotStandby();

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

int ChargeController::getDesiredCurrent()
{
    return this->desiredCurrent;
}

float ChargeController::getActualCurrent()
{
    return ((analogRead(PIN_CT_IN) - 511.0) / 512.0) * 32; // TODO This is just imaginary
}

void ChargeController::onVehicleStateChange(EventHandler handler)
{
    this->vehicleStateChange = handler;
}

void ChargeController::onStateChange(EventHandler handler)
{
    this->stateChange = handler;
}
