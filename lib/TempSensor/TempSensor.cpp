#include "TempSensor.h"

#include <Arduino.h>

// We assume a standard 10K (+/- 5%) NTC thermistor (R1) that is placed in a voltage divider with another resistor (R2) of the same value (10K):
#define R2 10000

// Reference voltages
#define VREF 5.0
#define VIN VREF

// Define the Steinhart-Hart coefficients for the used NTC (derived from datasheet for type "MF5A-3" thermistor)
// See https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html
#define A .001035859558
#define B .0002489452793
#define C .000000003236347528

/**
 * Calculates the temperature of a thermistor using Steinhart-Hart equation.
 * 
 * A, B, and C are the Steinhart–Hart coefficients, which vary depending on the type and model of thermistor.
 * 
 * See https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation.
*/
float steinhartHart(float a, float b, float c, float r) {
    float logR = log(r);
    return 1.0 / (a + b * logR + c * pow(logR, 3));
}

float kelvinToCelcius(float K) {
    return K - 273.15;
}

float voltageDivider(float vin, float vout, float r2) {
    return r2 * (vin / vout - 1);
}

unsigned long lastRead = 0;

TempSensor::TempSensor(uint8_t pinNumber)
{
    this->pinNumber = pinNumber;
}

float TempSensor::read()
{
    float pinVoltage = VREF * (analogRead(this->pinNumber) / 1023.0);
    float resistance = voltageDivider(VIN, pinVoltage, R2);
    float tempKelvin = steinhartHart(A, B, C, resistance);
    this->lastReadTemp = kelvinToCelcius(tempKelvin);
    
    return this->lastReadTemp;
}

float TempSensor::getLastRead()
{
    return this->lastReadTemp;
}
