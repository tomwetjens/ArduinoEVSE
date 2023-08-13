#include "RgbLed.h"

RgbLed::RgbLed(uint8_t redPinNumber, uint8_t greenPinNumber, uint8_t bluePinNumber)
{
    this->redPinNumber = redPinNumber;
    this->greenPinNumber = greenPinNumber;
    this->bluePinNumber = bluePinNumber;
}

void RgbLed::setColor(Color color)
{
    setRed(color == RED ? HIGH : LOW);
    setGreen(color == GREEN ? HIGH : LOW);
    setBlue(color == BLUE ? HIGH : LOW);
}

void RgbLed::setRed(int status)
{
    digitalWrite(redPinNumber, status);
}

void RgbLed::setGreen(int status)
{
    digitalWrite(greenPinNumber, status);
}

void RgbLed::setBlue(int status)
{
    digitalWrite(bluePinNumber, status);
}
