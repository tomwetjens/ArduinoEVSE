#include <Arduino.h>

#include "Display.h"
#include "ChargeController.h"
#include "NetworkManager.h"
#include "MqttController.h"

#define LED_GREEN 13
#define LED_RED 5

ChargeController chargeController;
NetworkManager networkManager;
MqttController mqttController(chargeController);
Display display(chargeController, networkManager, mqttController);

void vehicleStateChanged()
{
  mqttController.sendUpdate();
}

void updateLED()
{
  State state = chargeController.getState();

  switch (state)
  {
  case Ready:
    if (mqttController.isConnected() && networkManager.isConnected())
    {
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
    }
    else
    {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
    }
    break;
  case Charging:
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    // Blue is soldered to charger relay
    break;
  case Error:
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    break;
  }
}

void stateChanged()
{
  updateLED();
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

  updateLED();
}

void loop()
{
  chargeController.update();
  display.update();
  networkManager.update();
  mqttController.loop();

  if (!mqttController.isConnected())
  {
    // For safety reasons, we have to fall back to a safe charging current
    if (chargeController.getState() == Charging && chargeController.getCurrentLimit() > 6)
    {
      Serial.println("MQTT disconnected. Falling back to safe charging current");
      chargeController.setCurrentLimit(6);
    }
  }

  updateLED();
}
