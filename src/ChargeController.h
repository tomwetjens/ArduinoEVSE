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
    Pilot pilot;
    VehicleState vehicleState;
    int maxCurrent;
    float currentLimit;
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
    float getCurrentLimit();
    void setCurrentLimit(float amps);
    Pilot* getPilot();
    void onVehicleStateChange(EventHandler handler);
    void onStateChange(EventHandler handler);
};

#endif // CHARGECONTROLLER_H_