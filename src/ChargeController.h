#ifndef CHARGECONTROLLER_H_  _
#define CHARGECONTROLLER_H_  _

#include "Pilot.h"

enum State
{
    Ready,
    Charging,
    Error
};

using EventHandler = void (*)();

class ChargeController
{
private:
    State state;
    int pilotPinRead;
    float pilotPinVoltage;
    float pilotVoltage;
    VehicleState vehicleState;
    int maxCurrent;
    int desiredCurrent;
    unsigned long startMillis;
    EventHandler vehicleStateChange;
    EventHandler stateChange;
    void updateVehicleState();
    void closeRelay();
    void openRelay();

public:
    void setup();
    void update();
    void startCharging();
    void stopCharging();
    State getState();
    VehicleState getVehicleState();
    unsigned long getElapsedTime();
    int getDesiredCurrent();
    float getActualCurrent();
    void onVehicleStateChange(EventHandler handler);
    void onStateChange(EventHandler handler);
};

#endif // CHARGECONTROLLER_H_