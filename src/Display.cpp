#include <LiquidCrystal_I2C.h>

#include "Display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long elapsedTime;
unsigned long lastActualCurrentMillis;

void Display::setup()
{
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("ArduinoEV");
}

void Display::showStatus(String status)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(status);
}

void Display::showDesiredCurrent(int amps)
{
    char buffer[4];
    sprintf(buffer, "%2d A", amps);

    lcd.setCursor(12, 0);
    lcd.print(buffer);
}

void Display::showActualCurrent(float amps)
{
    if (millis() - lastActualCurrentMillis >= 1000)
    {
        float frac = amps - (int)amps;
        int decimals = frac * 10;

        char buffer[6];
        sprintf(buffer, "%2d.%01d A", (int)amps, decimals);

        lcd.setCursor(10, 1);
        lcd.print(buffer);

        lastActualCurrentMillis = millis();
    }
}

void Display::showElapsedTime(unsigned long millis)
{
    if (millis - elapsedTime >= 1000)
    {
        elapsedTime = millis;

        int hours = millis / 3600000;
        int minutes = (millis % 3600000) / 60000;
        int seconds = (millis % 60000) / 1000;

        char buffer[9];
        sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);

        lcd.setCursor(0, 1);
        lcd.print(buffer);
    }
}
