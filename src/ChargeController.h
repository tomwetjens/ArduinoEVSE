enum State
{
    Ready,
    Charging,
    Error
};

enum VehicleState
{
    EV_NotConnected,
    EV_Connected,
    EV_Ready,
    EV_ReadyVentilationRequired,
    EV_NoPower,
    EV_Error
};

using EventHandler = void (*)();

class ChargeController
{
private:
    State state;
    VehicleState vehicleState;
    int maxCurrent;
    int desiredCurrent;
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
    int getDesiredCurrent();
    float getActualCurrent();
    void onVehicleStateChange(EventHandler handler);
    void onStateChange(EventHandler handler);
};