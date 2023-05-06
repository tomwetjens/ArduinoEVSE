#include <unity.h>
#include <ArduinoFake.h>

using namespace fakeit;

#include <Pilot.h>
#include <TempSensor.h>
#include <ChargeController.h>
#include <MainsMeter.h>
#include <LoadBalancing.h>

Pilot pilot;
// TempSensor tempSensor(PIN_A2);
// ChargeController chargeController(pilot, tempSensor);
// MainsMeter mainsMeter;
// LoadBalancing loadBalancing(chargeController, mainsMeter);

void setUp(void)
{
    ArduinoFakeReset();

    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *))).AlwaysReturn();

    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const __FlashStringHelper *))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const String &s))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char[]))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(double, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const Printable &))).AlwaysReturn();

    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const __FlashStringHelper *))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const String &s))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char[]))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(char))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned char, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned int, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(long long, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(unsigned long long, int))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(double, int))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const Printable &))).AlwaysReturn();
    // When(OverloadedMethod(ArduinoFake(Serial), println, size_t(void))).AlwaysReturn();

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

void test_loadbalancing()
{
    // When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

    // loadBalancing.setup({});
    // loadBalancing.loop();
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_pilot_read);
    RUN_TEST(test_pilot_standby);
    RUN_TEST(test_pilot_currentLimit);
    RUN_TEST(test_loadbalancing);

    UNITY_END();
}
