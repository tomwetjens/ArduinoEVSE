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

#ifndef RGB_LED_H
#define RGB_LED_H _

#include <Arduino.h>

enum Color {
    OFF,
    RED,
    GREEN, 
    BLUE
};

class RgbLed
{
private:
  uint8_t redPinNumber;
  uint8_t greenPinNumber;
  uint8_t bluePinNumber;

public:
  RgbLed(uint8_t redPinNumber, uint8_t greenPinNumber, uint8_t bluePinNumber);
  void setColor(Color color);
};

#endif // RGB_LED_H