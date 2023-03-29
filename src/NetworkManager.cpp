#include "NetworkManager.h"

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"

unsigned long lastCheckMillis = 0;
int lastStatus = WL_IDLE_STATUS;

void connect()
{
    Serial.print("Connecting to WiFi network: ");
    Serial.println(WIFI_SSID);

    WiFi.disconnect();

    // Attempt to connect to Wifi network:
    lastStatus = WiFi.begin(WIFI_SSID, WIFI_WPA_PASS);
    lastCheckMillis = millis();
}

void NetworkManager::setup()
{
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");

        // don't continue
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();
    Serial.print("WiFi firmware version: ");
    Serial.println(fv);

    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.print("Please upgrade the firmware! Latest version: ");
        Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
    }

    WiFi.setTimeout(100); // Low timeout value to not really block on WiFi.begin()
    WiFi.setHostname("ArduinoEVSE");

    // TODO Read SSID and password from EEPROM
    // TODO If no SSID configured, then start AP instead of connect
    connect();
}

String getStatusText(int status)
{
    switch (status)
    {
    case WL_IDLE_STATUS:
        return "Not connected";
    case WL_CONNECTED:
        return "Connected";
    case WL_SCAN_COMPLETED:
        return "Scan completed";
    case WL_CONNECT_FAILED:
        return "Connect failed"; // Wrong SSID
    case WL_NO_SSID_AVAIL:
        return "No SSID available";
    case WL_CONNECTION_LOST:
        return "Connection lost";
    case WL_DISCONNECTED:
        return "Disconnected"; // Wrong password
    default:
        return "Unknown";
    }
}

void NetworkManager::loop()
{
    if (millis() - lastCheckMillis >= 2000)
    {
        lastCheckMillis = millis();

        int status = WiFi.status();

        if (lastStatus != status)
        {
            lastStatus = status;

            Serial.print("WiFi status: ");
            Serial.println(getStatusText(status));

            switch (status)
            {
            case WL_CONNECTED:
                IPAddress ip = WiFi.localIP();
                Serial.print("IP address: ");
                Serial.println(ip);
                Serial.print("Signal strength: ");
                Serial.print(WiFi.RSSI());
                Serial.println("dBm");

                if (this->connected)
                {
                    this->connected();
                }
                break;

            case WL_CONNECTION_LOST:
            case WL_DISCONNECTED:
                if (this->disconnected)
                {
                    this->disconnected();
                }
                break;
            }
        }

        switch (status)
        {
        case WL_CONNECTION_LOST:
        case WL_DISCONNECTED:
        case WL_CONNECT_FAILED:
            connect();
            break;
        }
    }
}

bool NetworkManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void NetworkManager::onConnected(EventHandler handler)
{
    this->connected = handler;
}

void NetworkManager::onDisconnected(EventHandler handler)
{
    this->disconnected = handler;
}
