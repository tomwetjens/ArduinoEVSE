#include <WiFiNINA.h>

class WebServer
{
    private:
      bool running;
public:
    void start();
    void stop();
    void loop();
};