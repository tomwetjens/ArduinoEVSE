#include <unity.h>
#include <ArduinoFake.h>

using namespace fakeit;

#include <Pilot.h>
#include <TempSensor.h>
#include <ChargeController.h>
#include <MainsMeter.h>
#include <LoadBalancing.h>

class PilotFake : public IPilot
{
private:
    VehicleState _vehicleState = VehicleNotConnected;
    float _currentLimit = -1;

public:
    VehicleState read() override { return this->_vehicleState; }
    void standby() override { this->_currentLimit = 0; }
    void currentLimit(float amps) override { this->_currentLimit = amps; };
    void set(VehicleState vehicleState) { this->_vehicleState = vehicleState; }
    bool isStandby() { return this->_currentLimit = 0; }
    float getCurrentLimit() { return this->_currentLimit; }
};

class TempSensorFake : public ITempSensor
{
private:
    float _temp = 20.0f;

public:
    float read() override { return this->_temp; }
    void set(float temp) { this->_temp = temp; }
};

Pilot pilot;
PilotFake pilotFake;

TempSensor tempSensor(PIN_A2);
TempSensorFake tempSensorFake;

ChargeController chargeController(pilotFake, tempSensorFake);

MainsMeter mainsMeter;

LoadBalancing loadBalancing(chargeController, mainsMeter);

void setUp(void)
{
    ArduinoFakeReset();

    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const __FlashStringHelper *))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const String &s))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char[]))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(double, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const Printable &))).AlwaysReturn();

    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const __FlashStringHelper *))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const String &s))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char[]))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(char))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned char, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned int, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(double, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const Printable &))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(void))).AlwaysReturn();

    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
}

void tearDown(void)
{
    // clean stuff up here
}

void test_pilot_read()
{
    // Pin    Pilot   Value  Result
    // 4.47V  12V     915    VehicleNotConnected
    // 4.31V  11V     882    VehicleNotConnected
    // 4.15V  10V     849    VehicleConnected
    // 3.99V  9V      816    VehicleConnected
    // 3.83V  8V      784    VehicleConnected
    // 3.67V  7V      751    VehicleReady
    // 3.51V  6V      718    VehicleReady
    // 3.35V  5V      686    VehicleReady
    // 3.19V  4V      653    VehicleReadyVentilationRequired
    // 3.03V  3V      620    VehicleReadyVentilationRequired
    // 2.87V  2V      588    VehicleReadyVentilationRequired
    // 2.71V  1V      557    VehicleNoPower
    // 2.55V  0V      522    VehicleError
    // 2.55V -12V     522    VehicleError

    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(915);
    TEST_ASSERT_EQUAL_UINT8(VehicleNotConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(915);
    TEST_ASSERT_EQUAL_UINT8(VehicleNotConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(882);
    TEST_ASSERT_EQUAL_UINT8(VehicleNotConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(849);
    TEST_ASSERT_EQUAL_UINT8(VehicleConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(816);
    TEST_ASSERT_EQUAL_UINT8(VehicleConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(784);
    TEST_ASSERT_EQUAL_UINT8(VehicleConnected, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(751);
    TEST_ASSERT_EQUAL_UINT8(VehicleReady, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(718);
    TEST_ASSERT_EQUAL_UINT8(VehicleReady, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(686);
    TEST_ASSERT_EQUAL_UINT8(VehicleReady, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(653);
    TEST_ASSERT_EQUAL_UINT8(VehicleReadyVentilationRequired, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(620);
    TEST_ASSERT_EQUAL_UINT8(VehicleReadyVentilationRequired, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(588);
    TEST_ASSERT_EQUAL_UINT8(VehicleReadyVentilationRequired, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(557);
    TEST_ASSERT_EQUAL_UINT8(VehicleNoPower, pilot.read());
    When(Method(ArduinoFake(), analogRead)).AlwaysReturn(522);
    TEST_ASSERT_EQUAL_UINT8(VehicleError, pilot.read());

    Verify(Method(ArduinoFake(), analogRead).Using(PIN_A1)).AtLeastOnce();
}

void test_pilot_standby()
{
    pilot.standby();

    Verify(Method(ArduinoFake(), digitalWrite).Using(10, 1)).Once();
}

void test_pilot_currentLimit()
{
    pilot.currentLimit(80);
    // TODO: Verify PWM duty cycle and frequency
    // TODO: Add test cases
}

void test_pilot_vehicleStateToText()
{
    char actual[50];

    vehicleStateToText(VehicleNotConnected, actual);
    TEST_ASSERT_EQUAL_STRING("Not connected", actual);

    vehicleStateToText(VehicleConnected, actual);
    TEST_ASSERT_EQUAL_STRING("Connected, not ready", actual);

    vehicleStateToText(VehicleReady, actual);
    TEST_ASSERT_EQUAL_STRING("Ready", actual);

    vehicleStateToText(VehicleReadyVentilationRequired, actual);
    TEST_ASSERT_EQUAL_STRING("Ready, ventilation required", actual);

    vehicleStateToText(VehicleNoPower, actual);
    TEST_ASSERT_EQUAL_STRING("No power", actual);

    vehicleStateToText(VehicleError, actual);
    TEST_ASSERT_EQUAL_STRING("Error", actual);
}

void test_loadbalancing_initially_safe_fallback_current()
{
    // Given
    When(Method(ArduinoFake(), millis)).Return(0, 20000, 20001, 20002, 20003, 20004, 20005, 20006, 20007, 20008, 20009, 20010, 20011, 20012, 20013, 20014, 20015, 20016, 20017, 20018, 20019, 20020, 20021);

    LoadBalancingSettings loadBalancingSettings;
    loadBalancingSettings.maxMainsCurrent = 25.0;
    loadBalancingSettings.fallbackCurrent = 6.0;
    loadBalancingSettings.fallbackTimeout = 20000;
    loadBalancing.setup(loadBalancingSettings);

    ChargingSettings chargingSettings;
    chargingSettings.maxCurrent = 16;
    chargeController.setup(chargingSettings);
    // Actual charging current has not been updated yet

    mainsMeter.setup();
    // Mains meter values have not been updated yet

    // When
    loadBalancing.loop();

    // Then
    TEST_ASSERT_EQUAL_FLOAT(6.0, chargeController.getCurrentLimit());
}

void test_loadbalancing_case(float mainsImportCurrent, float mainsExportCurrent, float actualChargingCurrent, float expectedCurrentLimit, char *message)
{
    // Given
    LoadBalancingSettings loadBalancingSettings;
    loadBalancingSettings.maxMainsCurrent = 25.0;
    loadBalancingSettings.fallbackCurrent = 6.0;
    loadBalancingSettings.fallbackTimeout = 20000;
    loadBalancing.setup(loadBalancingSettings);

    ChargingSettings chargingSettings;
    chargingSettings.maxCurrent = 16;
    chargeController.setup(chargingSettings);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(16, chargeController.maxCurrent(), "Charging max current"); // Sanity check

    // When
    mainsMeter.updateValues({mainsImportCurrent, 0, 0}, {mainsExportCurrent, 0, 0});
    chargeController.updateActualCurrent({actualChargingCurrent, 0, 0});

    loadBalancing.loop();

    // Then
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expectedCurrentLimit, chargeController.getCurrentLimit(), message);
}

void test_loadbalancing()
{
    // Given
    When(Method(ArduinoFake(), millis)).Return(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50);

    test_loadbalancing_case(0.0, 0.0, 0.0, 16.0, "Not charging, no excess, no house load");
    test_loadbalancing_case(1.4, 0.0, 0.0, 16.0, "Not charging, no excess, house load");
    test_loadbalancing_case(0.0, 1.4, 0.0, 16.0, "Not charging, excess available");
    test_loadbalancing_case(0.0, 1.4, 15.9, 16.0, "Fully charging from excess, with still excess left");
    test_loadbalancing_case(17.6, 0.0, 15.9, 16.0, "House load, but not at mains max current yet");
    test_loadbalancing_case(25.0, 0.0, 15.9, 15.9, "At mains max current");
    test_loadbalancing_case(25.0, 0.0, 16.0, 16.0, "At mains max current and at max charging current");
    test_loadbalancing_case(28.0, 0.0, 15.9, 12.9, "Mains max current exceeded");
}

void test_loadbalancing_current_limit_determined_externally()
{
    // Given
    When(Method(ArduinoFake(), millis)).Return(0, 0, 1, 2, 3, 4, 5);

    LoadBalancingSettings loadBalancingSettings;
    loadBalancingSettings.maxMainsCurrent = 25.0;
    loadBalancingSettings.fallbackCurrent = 6.0;
    loadBalancingSettings.fallbackTimeout = 20000;
    loadBalancing.setup(loadBalancingSettings);

    ChargingSettings chargingSettings;
    chargingSettings.maxCurrent = 16;
    chargeController.setup(chargingSettings);

    mainsMeter.setup();

    // When
    loadBalancing.setCurrentLimit(10.0);
    loadBalancing.loop();

    // Then
    TEST_ASSERT_EQUAL_FLOAT(10.0, chargeController.getCurrentLimit());
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // RUN_TEST(test_pilot_read);
    // RUN_TEST(test_pilot_standby);
    // RUN_TEST(test_pilot_currentLimit);
    // RUN_TEST(test_pilot_vehicleStateToText);

    RUN_TEST(test_loadbalancing_initially_safe_fallback_current);
    // RUN_TEST(test_loadbalancing);
    // RUN_TEST(test_loadbalancing_current_limit_determined_externally);

    UNITY_END();
}
