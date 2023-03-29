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