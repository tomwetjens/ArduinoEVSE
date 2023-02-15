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
      float readVoltage();
    public:
      VehicleState read();
      void standby();
      void currentLimit(int amps);
};

#endif // PILOT_H_