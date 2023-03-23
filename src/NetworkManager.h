#ifndef NETWORKMANAGER_H_  _
#define NETWORKMANAGER_H_  _

using EventHandler = void (*)();

class NetworkManager
{
private:
    EventHandler connected;
    EventHandler disconnected;

public:
    void setup();
    void update();
    bool isConnected();
    void onConnected(EventHandler handler);
    void onDisconnected(EventHandler handler);
};

#endif