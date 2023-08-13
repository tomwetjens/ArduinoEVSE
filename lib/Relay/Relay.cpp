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

#include <Relay.h>

Relay::Relay(pin_size_t pin)
{
    this->_pin = pin;
}

void Relay::setup(unsigned long delay, int initialState)
{
    _delay = delay;

    pinMode(_pin, OUTPUT);

    _desiredState = initialState;
    digitalWrite(_pin, initialState);
    _currentState = initialState;
}

void Relay::openImmediately()
{
    _desiredState = 0;
    digitalWrite(_pin, 0);
    _currentState = 0;
}

void Relay::open()
{
    if (_desiredState != 0)
    {
        _desiredState = 0;
        _lastCalledMillis = millis();
    }
}

void Relay::close()
{
    if (_desiredState != 1)
    {
        _desiredState = 1;
        _lastCalledMillis = millis();
    }
}

void Relay::loop()
{
    if (_currentState != _desiredState)
    {
        if (millis() - _lastCalledMillis >= _delay)
        {
            digitalWrite(_pin, _desiredState);
            _currentState = _desiredState;
        }
    }
}
