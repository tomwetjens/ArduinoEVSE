#include <Arduino.h>

#include "Display.h"
#include "ChargeController.h"
#include "NetworkManager.h"
#include "MqttController.h"

#define LED_GREEN 13
#define LED_RED 5

ChargeController chargeController;
Display display(chargeController);
NetworkManager networkManager;
MqttController mqttController(chargeController);

void vehicleStateChanged()
{
  mqttController.sendUpdate();
}

void updateLED(byte red, byte green, byte blue)
{
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  // Blue is soldered to charger relay
}

void stateChanged()
{
  State state = chargeController.getState();

  switch (state)
  {
  case Ready:
    updateLED(0, 1, 0);
    break;
  case Charging:
    updateLED(0, 0, 1);
    break;
  case Error:
    updateLED(1, 0, 0);
    break;
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("ArduinoEVSE");

  display.setup();
  chargeController.setup();
  networkManager.setup();
  mqttController.setup();

  chargeController.onStateChange(stateChanged);
  chargeController.onVehicleStateChange(vehicleStateChanged);

  stateChanged();
}

void loop()
{
  chargeController.update();
  
  display.update();

  networkManager.update();

  mqttController.loop();
}
