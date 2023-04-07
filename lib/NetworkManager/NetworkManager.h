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

#ifndef NETWORKMANAGER_H_  _
#define NETWORKMANAGER_H_  _

using EventHandler = void (*)();

struct WiFiSettings
{
    char ssid[100];
    char password[100];
};

class NetworkManager
{
private:
    WiFiSettings settings;
    EventHandler connected;
    EventHandler disconnected;
    void connect();

public:
    void setup(WiFiSettings settings);
    void loop();

    bool isConnected();

    // Event handlers
    void onConnected(EventHandler handler);
    void onDisconnected(EventHandler handler);
};

#endif // NETWORKMANAGER_H_