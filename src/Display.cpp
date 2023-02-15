#include <LiquidCrystal_I2C.h>

#include "Display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

Display::Display(ChargeController &chargeController)
{
    this->chargeController = &chargeController;
}

void Display::setup()
{
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ArduinoEVSE");
}

void Display::update()
{
    State state = this->chargeController->getState();
    unsigned long now = millis();

    if (state != lastUpdateState || now - lastUpdateMillis >= 1000)
    {
        switch (state)
        {
        case Ready:
            this->printStatus("Ready");
            break;
        case Charging:
            this->printStatus("Charging");
            this->printElapsedTime();
            this->printDesiredCurrent(this->chargeController->getDesiredCurrent());
            this->printActualCurrent(this->chargeController->getActualCurrent());
            break;
        case Error:
            this->printStatus("Error");
            break;
        }

        lastUpdateState = state;
        lastUpdateMillis = now;
    }
}

void Display::printStatus(String status)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(status);
}

void Display::printDesiredCurrent(int amps)
{
    char buffer[4];
    sprintf(buffer, "%2d A", amps);

    lcd.setCursor(12, 0);
    lcd.print(buffer);
}

void Display::printActualCurrent(float amps)
{
    float frac = amps - (int)amps;
    int decimals = frac * 10;

    char buffer[6];
    sprintf(buffer, "%2d.%01d A", (int)amps, decimals);

    lcd.setCursor(10, 1);
    lcd.print(buffer);
}

void Display::printElapsedTime()
{
    unsigned long elapsedTime = this->chargeController->getElapsedTime();

    int hours = elapsedTime / 3600000;
    int minutes = (elapsedTime % 3600000) / 60000;
    int seconds = (elapsedTime % 60000) / 1000;

    char buffer[9];
    sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);

    lcd.setCursor(0, 1);
    lcd.print(buffer);
}
