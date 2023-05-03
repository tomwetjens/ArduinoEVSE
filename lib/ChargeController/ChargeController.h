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

#ifndef CHARGECONTROLLER_H_
#define CHARGECONTROLLER_H_ _

#include "Pilot.h"
#include <TempSensor.h>

enum State
{
  Ready,
  Charging,
  Error,
  Overheat
};

typedef void (*ChargeControllerEventHandler)();

struct ChargingSettings
{
  // Max current (A) that installed cabling can handle and must never be exceeded
  uint8_t maxCurrent = 16;

  // Temperature (C) at which charger is considered overheated - will immediately stop charging and must be reset to charge again
  uint8_t overheatTemp = 70;
};

class ChargeController
{
private:
  ChargingSettings settings;
  State state;
  Pilot *pilot;
  TempSensor *tempSensor;
  VehicleState vehicleState;
  float currentLimit;
  float _actualCurrent;
  unsigned long _actualCurrentUpdated;
  unsigned long started;
  ChargeControllerEventHandler vehicleStateChange;
  ChargeControllerEventHandler stateChange;
  void updateVehicleState();
  void detectOverheat();
  void applyCurrentLimit();
  void closeRelay();
  void openRelay();

public:
  ChargeController(Pilot &pilot, TempSensor &tempSensor);

  void setup(ChargingSettings settings);
  void loop();

  void startCharging();
  void stopCharging();

  State getState();
  VehicleState getVehicleState();
  unsigned long getElapsedTime();

  const float &maxCurrent = this->settings.maxCurrent;

  float getCurrentLimit();
  void setCurrentLimit(float currentLimit);

  const float &actualCurrent = _actualCurrent;
  const unsigned long &actualCurrentUpdated = _actualCurrentUpdated;
  void updateActualCurrent(float actualCurrent);

  Pilot *getPilot();

  float getTemp();

  // Event handlers
  void onVehicleStateChange(ChargeControllerEventHandler handler);
  void onStateChange(ChargeControllerEventHandler handler);
};

#endif // CHARGECONTROLLER_H_