#include "RgbLed.h"

RgbLed::RgbLed(uint8_t redPinNumber, uint8_t greenPinNumber, uint8_t bluePinNumber)
{
    this->redPinNumber = redPinNumber;
    this->greenPinNumber = greenPinNumber;
    this->bluePinNumber = bluePinNumber;
}

void RgbLed::setColor(Color color)
{
    digitalWrite(redPinNumber, color == RED ? HIGH : LOW);
    digitalWrite(greenPinNumber, color == GREEN ? HIGH : LOW);
    digitalWrite(bluePinNumber, color == BLUE ? HIGH : LOW);
}
