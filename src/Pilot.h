#ifndef PILOT_H_  _
#define PILOT_H_  _

enum VehicleState
{
    EV_NotConnected,
    EV_Connected,
    EV_Ready,
    EV_ReadyVentilationRequired,
    EV_NoPower,
    EV_Error
};

class Pilot {
    private:
      float lastPilotVoltage;
      float lastPinVoltage;
      float readPin();
    public:
      VehicleState read();
      void standby();
      void currentLimit(float amps);
      float getLastPilotVoltage();
      float getLastPinVoltage();
};

#endif // PILOT_H_