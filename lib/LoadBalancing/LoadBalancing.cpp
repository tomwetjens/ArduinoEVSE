#include "LoadBalancing.h"

#include <math.h>

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

float LoadBalancing::calculateMaxImportCurrent()
{
    // Determine the current load on the mains (highest of the 3 phases)
    auto importCurrent = mainsMeter->importCurrent();
    float mainsLoad = fmax(0, fmax(importCurrent.l1, fmax(importCurrent.l2, importCurrent.l3)));

    // Take out the load generated by the charger itself currently
    auto actualCurrent = chargeController->actualCurrent();
    float houseLoad = mainsLoad - fmax(actualCurrent.l1, fmax(actualCurrent.l2, actualCurrent.l3)); // can be negative, meaning some power is also generated

    // Calculate max current charger may take, giving priority to the house load
    return fmax(0, settings.maxMainsCurrent - houseLoad);
}

void LoadBalancing::balanceLoad()
{
    unsigned long now = millis();

    if (settings.enabled && now - lastChecked >= 1000)
    {
        // Only when mains meter values are recent (and not initial)
        if (now - mainsMeter->updated() < settings.meterTimeout && mainsMeter->updated() > 0)
        {
            // And actual charging current value is recent (and not initial)
            if (now - chargeController->actualCurrentUpdated() < settings.meterTimeout && chargeController->actualCurrentUpdated() > 0)
            {
                float maxImportCurrent = calculateMaxImportCurrent();

                setCurrentLimit(maxImportCurrent);
            }
            else
            {
                Serial.print("WARN: Actual current updated ");
                Serial.print(now - chargeController->actualCurrentUpdated());
                Serial.println(" ms ago");
            }
        }
        else
        {
            Serial.print("WARN: Mains meter values updated ");
            Serial.print(now - mainsMeter->updated());
            Serial.println(" ms ago");
        }

        lastChecked = now;
    }
}

bool LoadBalancing::fallbackEnabled()
{
    return settings.fallbackTimeout > 0;
}

void LoadBalancing::fallbackCurrentIfOutdated()
{
    if (fallbackEnabled())
    {
        // When state is initial or current limit is outdated
        auto now = millis();
        if (now - currentLimitLastUpdated >= settings.fallbackTimeout)
        {
            if (chargeController->getCurrentLimit() > settings.fallbackCurrent)
            {
                // Fall back to a safe charging current (for safety reasons)
                Serial.println("WARN: Current limit updated ");
                Serial.print(now - currentLimitLastUpdated);
                Serial.println(" ms ago. Falling back to safe charging current");
                fallbackCurrent();
            }
        }
    }
}

void LoadBalancing::fallbackCurrent()
{
    // When not already at a safe current
    if (chargeController->getCurrentLimit() > settings.fallbackCurrent)
    {
        Serial.print("Setting safe charging current limit: ");
        Serial.println(settings.fallbackCurrent);

        chargeController->setCurrentLimit(settings.fallbackCurrent);
    }
}

LoadBalancing::LoadBalancing(ChargeController &chargeController, MainsMeter &mainsMeter)
{
    this->chargeController = &chargeController;
    this->mainsMeter = &mainsMeter;
}

void LoadBalancing::setup(LoadBalancingSettings settings)
{
    this->settings = settings;

    currentLimitLastUpdated = 0;
    lastChecked = 0;

    if (fallbackEnabled())
    {
        fallbackCurrent();
    }
}

void LoadBalancing::loop()
{
    balanceLoad();
    fallbackCurrentIfOutdated();
}

void LoadBalancing::setCurrentLimit(float amps)
{
    // TODO: Increasing the charging current limit should be smoothed to prevent too much relay switching (decreasing should be immediate, for safety)
    chargeController->setCurrentLimit(amps);
    currentLimitLastUpdated = millis();
}
