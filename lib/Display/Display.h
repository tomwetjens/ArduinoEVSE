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

#ifndef DISPLAY_H_
#define DISPLAY_H_ _

#include "ChargeController.h"
#include "NetworkManager.h"
#include "MqttController.h"

class Display
{
private:
    ChargeController *chargeController;
    NetworkManager *networkManager;
    MqttController *mqttController;
    State lastUpdateState;
    unsigned long lastUpdated;
    void printStatus(String status);
    void printCurrentLimit(float amps);
    void printElapsedTime();

public:
    Display(ChargeController &chargeController,
            NetworkManager &networkManager,
            MqttController &mqttController);
    void setup();
    void loop();
};

#endif // DISPLAY_H_