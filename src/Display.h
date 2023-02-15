#ifndef DISPLAY_H_ _
#define DISPLAY_H_ _

#include "ChargeController.h"

class Display
{
private:
    ChargeController* chargeController;
    State lastUpdateState;
    unsigned long lastUpdateMillis;
    void printStatus(String status);
    void printActualCurrent(float amps);
    void printCurrentLimit(float amps);
    void printElapsedTime();

public:
    Display(ChargeController &chargeController);
    void setup();
    void update();
};

#endif