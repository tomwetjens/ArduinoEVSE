/*
 * This file is part of the ArduinoEVSE (https://github.com/tomwetjens/ArduinoEVSE).
 * Copyright (c) 2023 Tom Wetjens.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_ _

#include <WiFiClient.h>

using EventHandler = void (*)();

struct WiFiSettings
{
    char ssid[33];
    char password[64];
};

class NetworkManager
{
private:
    WiFiSettings settings;
    EventHandler connected;
    EventHandler disconnected;
    void connect();
    WiFiClient client;

public:
    NetworkManager();
    void setup(WiFiSettings settings);
    void loop();

    bool isConnected();
    WiFiClient& getClient();

    // Event handlers
    void onConnected(EventHandler handler);
    void onDisconnected(EventHandler handler);
};

#endif // NETWORKMANAGER_H_