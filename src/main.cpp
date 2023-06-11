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

#include <RgbLed.h>
#include <Display.h>
#include <ChargeController.h>
#include <NetworkManager.h>
#include <MqttController.h>
#include <TempSensor.h>
#include <LoadBalancing.h>
#include <MainsMeter.h>

#include "arduino_secrets.h"

#define PIN_D5 5
#define PIN_D8 8
#define PIN_D13 13

RgbLed rgbLed(PIN_D5, PIN_D13, PIN_D8);
Pilot pilot;
TempSensor tempSensor(PIN_A2);
ChargeController chargeController(pilot, tempSensor);
MainsMeter mainsMeter;
LoadBalancing loadBalancing(chargeController, mainsMeter);
NetworkManager networkManager;
MqttController mqttController(networkManager, pilot, chargeController, loadBalancing, mainsMeter);
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
      rgbLed.setColor(GREEN);
    }
    else
    {
      rgbLed.setColor(RED);
    }
    break;
  case Charging:
    rgbLed.setColor(BLUE);
    // Blue is also soldered to charger relay
    break;
  case Error:
    rgbLed.setColor(RED);
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("ArduinoEVSE");

  display.setup();
  mainsMeter.setup();
  chargeController.setup({});
  loadBalancing.setup({});

  struct WiFiSettings wifiSettings;
  strncpy(wifiSettings.ssid, WIFI_SSID, 33);
  strncpy(wifiSettings.password, WIFI_WPA_PASS, 64);
  networkManager.setup(wifiSettings);

  struct MqttSettings mqttSettings;
  strncpy(mqttSettings.host, MQTT_HOST, 254);
  mqttController.setup(mqttSettings);

  chargeController.onVehicleStateChange(vehicleStateChanged);

  updateLED();
}

void loop()
{
  chargeController.loop();
  loadBalancing.loop();
  display.loop();
  networkManager.loop();
  mqttController.loop();

  updateLED();
}
