using EventHandler = void (*)();

class NetworkManager
{
private:
    EventHandler connected;
    EventHandler disconnected;

public:
    void setup();
    void loop();
    void onConnected(EventHandler handler);
    void onDisconnected(EventHandler handler);
};