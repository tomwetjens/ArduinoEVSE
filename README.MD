# ArduinoEVSE

Tested with Arduino Uno WiFi Rev2.
Inspired by https://github.com/OpenEVSE and https://www.instructables.com/Arduino-EV-J1772-Charging-Station/.


## Build

Create a file `src/arduino_secrets.h`:
```
#define WIFI_SSID "myssid"
#define WIFI_WPA_PASS "mypass"
```

Use PlatformIO to build and upload to Arduino.

## MQTT

### MQTT Commands

Start Charging
Message: `0`

Stop Charging
Message: `1`

Set Current Limit
Message: `2,16.0`
