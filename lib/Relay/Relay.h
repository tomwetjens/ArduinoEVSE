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

#ifndef RELAY_H_
#define RELAY_H_ _

#include <Arduino.h>

class Relay
{
private:
  unsigned int _pin;
  unsigned long _delay;
  bool _currentState;
  bool _desiredState;
  unsigned long _lastCalledMillis;

public:
  Relay(pin_size_t pin);

  void setup(unsigned long delay, int initialState);
  void loop();

  void openImmediately();
  void open();
  void close();
};

#endif // RELAY_H_