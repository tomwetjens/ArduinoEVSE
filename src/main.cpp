#include <Arduino.h>

#include "Display.h"
#include "ChargeController.h"
#include "NetworkManager.h"
#include "WebServer.h"

#define LED_GREEN 13
#define LED_RED 5

Display display;
ChargeController chargeController;
NetworkManager networkManager;
WebServer webServer;

unsigned long chargingStartMillis;

void startChargingAutomatically()
{
  if (chargeController.getVehicleState() == EV_Ready)
  {
    delay(1000);
    chargeController.startCharging();
    chargingStartMillis = millis();
  }
}

void updateLED(byte red, byte green, byte blue)
{
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  // Blue is soldered to charger relay
}

void updateStatus()
{
  State state = chargeController.getState();

  switch (state)
  {
  case Ready:
    updateLED(0, 1, 0);
    display.showStatus("Ready");
    break;
  case Charging:
    updateLED(0, 0, 1);
    display.showStatus("Charging");
    break;
  case Error:
    updateLED(1, 0, 0);
    display.showStatus("Error");
    break;
  }
}

void startNetworkServices()
{
  webServer.start();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("ArduinoEVSE");

  display.setup();
  chargeController.setup();
  networkManager.setup();

  chargeController.onStateChange(updateStatus);
  chargeController.onVehicleStateChange(startChargingAutomatically);

  networkManager.onConnected(startNetworkServices);

  updateStatus();
}

void loop()
{
  chargeController.update();

  if (chargeController.getState() == Charging)
  {
    display.showElapsedTime(millis() - chargingStartMillis);
    display.showDesiredCurrent(chargeController.getDesiredCurrent());
    display.showActualCurrent(chargeController.getActualCurrent());
  }

  networkManager.loop();

  webServer.loop();
}
