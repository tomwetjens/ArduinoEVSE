enum State
{
    Ready,
    Charging,
    Overheat,
    Error
};

class EVSE
{
private:
    State state;

public:
    void setup();
    void loop();
    void setMaxCurrent(int maxCurrent);
};