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

#include "Display.h"
#include "ChargeController.h"
#include "NetworkManager.h"
#include "MqttController.h"

#define LED_GREEN 13
#define LED_RED 5

ChargeController chargeController;
NetworkManager networkManager;
MqttController mqttController(chargeController);
Display display(chargeController, networkManager, mqttController);

void vehicleStateChanged()
{
  mqttController.sendUpdate();
}

void updateLED()
{
  State state = chargeController.getState();

  switch (state)
  {
  case Ready:
    if (mqttController.isConnected() && networkManager.isConnected())
    {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
    }
    else
    {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
    }
    break;
  case Charging:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    // Blue is soldered to charger relay
    break;
  case Error:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    break;
  }
}

void stateChanged()
{
  updateLED();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("ArduinoEVSE");

  display.setup();
  chargeController.setup({});
  networkManager.setup();
  mqttController.setup({});

  chargeController.onStateChange(stateChanged);
  chargeController.onVehicleStateChange(vehicleStateChanged);

  updateLED();
}

void loop()
{
  chargeController.loop();
  display.loop();
  networkManager.loop();
  mqttController.loop();

  updateLED();
}
