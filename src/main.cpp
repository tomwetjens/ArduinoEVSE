#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <EVSE.h>

EVSE evse;

void setup()
{
  // put your setup code here, to run once:
  evse.setup();
}

void loop()
{
  // put your main code here, to run repeatedly:
  evse.loop();
}