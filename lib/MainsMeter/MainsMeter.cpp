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
#include <MainsMeter.h>

#include <Arduino.h>

void MainsMeter::setup()
{
    _importCurrent = {INT16_MAX, INT16_MAX, INT16_MAX}; // Some nonsense value to indicate it's unknown, but 'safe' when it's used anyway
    _exportCurrent = {0, 0, 0};
    _updated = 0;
}

void MainsMeter::updateValues(ThreePhases importCurrent, ThreePhases exportCurrent)
{
    Serial.print("Updating mains meter values: import: ");
    Serial.print(importCurrent.l1);
    Serial.print(" ");
    Serial.print(importCurrent.l2);
    Serial.print(" ");
    Serial.print(importCurrent.l3);
    Serial.print(" export: ");
    Serial.print(exportCurrent.l1);
    Serial.print(" ");
    Serial.print(exportCurrent.l2);
    Serial.print(" ");
    Serial.println(exportCurrent.l3);

    _importCurrent = importCurrent;
    _exportCurrent = exportCurrent;
    _updated = millis();
}