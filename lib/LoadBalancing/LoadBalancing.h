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

#ifndef LOAD_BALANCING_H_
#define LOAD_BALANCING_H_ _

#include <Arduino.h>

#include <ChargeController.h>
#include <MainsMeter.h>

struct LoadBalancingSettings
{
    // Max mains current
    uint8_t maxMainsCurrent = 25;

    // Safe fallback (A) current when no current limit is set within timeout (for load balancing)
    // If set to <6A, fallback will mean that charging will stop temporarily
    uint8_t fallbackCurrent = 0;

    // Timeout (milliseconds) after which charging will fall back to safe current, when a current limit could not been calculated (or was not received)
    // If set to 0, no fallback will be performed
    uint16_t fallbackTimeout = 20000;

    // Timeout (milliseconds) after which meter values are considered outdated and cannot be used for load balancing
    uint16_t meterTimeout = 10000;
};

class LoadBalancing
{
private:
    LoadBalancingSettings settings;
    ChargeController *chargeController;
    MainsMeter *mainsMeter;
    unsigned long lastChecked;
    unsigned long currentLimitLastUpdated;
    float calculateCurrentLimit();
    void balanceLoad();
    void fallbackCurrentIfNeeded();
public:
    LoadBalancing(ChargeController &chargeController, MainsMeter &mainsMeter);
    void setup(LoadBalancingSettings settings);
    void loop();
    void setCurrentLimit(float amps);
};

#endif // LOAD_BALANCING_H_