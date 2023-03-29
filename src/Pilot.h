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

#ifndef PILOT_H_  _
#define PILOT_H_  _

#define MIN_CURRENT 6
#define MAX_CURRENT 80

enum VehicleState
{
    VehicleNotConnected,
    VehicleConnected,
    VehicleReady,
    VehicleReadyVentilationRequired,
    VehicleNoPower,
    VehicleError
};

String vehicleStateToText(VehicleState vehicleState);

class Pilot {
    private:
      float pilotVoltage;
      float pinVoltage;
      float readPin();
    public:
      VehicleState read();
      void standby();
      void currentLimit(float amps);
      float getLastPilotVoltage();
      float getLastPinVoltage();
};

#endif // PILOT_H_