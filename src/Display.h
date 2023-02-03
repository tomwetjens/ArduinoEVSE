class Display
{
public:
    void setup();
    void showStatus(String status);
    void showActualCurrent(float amps);
    void showDesiredCurrent(int amps);
    void showElapsedTime(unsigned long millis);
};
