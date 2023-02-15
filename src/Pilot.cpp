#include <Arduino.h>
#include "megaAVR_PWM.h"

#include "Pilot.h"

#define PILOT_FREQUENCY 1000
#define PILOT_STANDBY_DUTY_CYCLE 99
#define PIN_PILOT_PWM 10

#define PIN_PILOT_IN_MIN_VOLTAGE 2.55 // @ 0V pilot, measured
#define PIN_PILOT_IN_MAX_VOLTAGE 4.33 // @ 12V pilot, measured
#define PIN_PILOT_IN PIN_A1

#define MIN_CURRENT 6
#define MAX_CURRENT 80

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
    digitalWrite(PIN_PILOT_PWM, HIGH); // +12V constant
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

    PWM_Instance->setPWM(PIN_PILOT_PWM, PILOT_FREQUENCY, dutyCycle);
}

float Pilot::readVoltage()
{
    int pinValue = analogReadMax(PIN_PILOT_IN, 10);                                                                  // 0-1023
    float pinVoltage = (pinValue / 1023.0) * 5.0;                                                                    // 0-5
    return ((pinVoltage - PIN_PILOT_IN_MIN_VOLTAGE) / (PIN_PILOT_IN_MAX_VOLTAGE - PIN_PILOT_IN_MIN_VOLTAGE)) * 12.0; // 0-12
}

VehicleState Pilot::read()
{
    float voltage = this->readVoltage();

    if (voltage >= 11) // 12V +/-1V
    {
        return EV_NotConnected;
    }
    else if (voltage >= 8) // 9V +/-1V
    {
        return EV_Connected;
    }
    else if (voltage >= 5) // 6V +/-1V
    {
        return EV_Ready;
    }
    else if (voltage >= 2) // 3V +/-1V
    {
        return EV_ReadyVentilationRequired;
    }
    else if (voltage >= 0) // 0V
    {
        return EV_NoPower;
    }
    else
    {
        return EV_Error; // -12V
    }
}