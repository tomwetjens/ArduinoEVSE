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

#ifndef MAINS_METER_H_
#define MAINS_METER_H_ _

struct ThreePhases
{
  float l1;
  float l2;
  float l3;
};

class MainsMeter
{
private:
  ThreePhases _importCurrent;
  ThreePhases _exportCurrent;
  unsigned long _updated;

public:
  void setup();
  
  const ThreePhases importCurrent() { return _importCurrent; }
  const ThreePhases exportCurrent() { return _exportCurrent; }
  const unsigned long updated() { return _updated; }

  void updateValues(ThreePhases importCurrent, ThreePhases exportCurrent);
};

#endif // MAINS_METER_H_