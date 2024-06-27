#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "Timer.hpp"
#include "DeviceMock.hpp"

// clang-format off
TEST_GROUP(TestTimer)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

TEST(TestTimer, ShouldCallCallbackOnTimeoutWithoutRepeating)  // NOLINT
{
    Timer<DeviceMock> timer;
    timer.setCallback([]{
        mock("Lambda").actualCall("callback");
    });

    mock("DeviceMock").expectOneCall("millis").andReturnValue(0);
    timer.start(100);

    // Do not call - too early
    mock("DeviceMock").expectOneCall("millis").andReturnValue(50);
    timer.update();

    // Time to call
    mock("DeviceMock").expectOneCall("millis").andReturnValue(100);
    mock("Lambda").expectOneCall("callback");
    timer.update();

    // Do not call again as repeating is disabled
    mock("DeviceMock").expectOneCall("millis").andReturnValue(200);
    mock("Lambda").expectNoCall("callback");
    timer.update();
}

TEST(TestTimer, ShouldCallCallbackOnTimeoutAndRepeate)  // NOLINT
{
    Timer<DeviceMock> timer;
    timer.setCallback([]{
        mock("Lambda").actualCall("callback");
    });

    mock("DeviceMock").expectOneCall("millis").andReturnValue(0);
    timer.start(100, true);

    // Do not call - too early
    mock("DeviceMock").expectOneCall("millis").andReturnValue(50);
    timer.update();

    // Time to call
    mock("DeviceMock").expectOneCall("millis").andReturnValue(100);
    mock("Lambda").expectOneCall("callback");
    timer.update();

    // Call again as should repeat
    mock("DeviceMock").expectOneCall("millis").andReturnValue(200);
    mock("Lambda").expectOneCall("callback");
    timer.update();
}

TEST(TestTimer, ShouldBeAbleToStopRepeating)  // NOLINT
{
    Timer<DeviceMock> timer;
    timer.setCallback([]{
        mock("Lambda").actualCall("callback");
    });

    mock("DeviceMock").expectOneCall("millis").andReturnValue(0);
    timer.start(100, true);

    // Do not call - too early
    mock("DeviceMock").expectOneCall("millis").andReturnValue(50);
    timer.update();

    // Time to call
    mock("DeviceMock").expectOneCall("millis").andReturnValue(100);
    mock("Lambda").expectOneCall("callback");
    timer.update();

    // Call again as should repeat
    mock("DeviceMock").expectOneCall("millis").andReturnValue(200);
    mock("Lambda").expectOneCall("callback");
    timer.update();

    timer.stop();

    // Do not call again as timer is stopped
    mock("DeviceMock").expectOneCall("millis").andReturnValue(300);
    mock("Lambda").expectNoCall("callback");
    timer.update();
}
