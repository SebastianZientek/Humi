#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <memory>

#include "MqttDeviceMock.hpp"
#include "MqttHumidifier.hpp"

// clang-format off
TEST_GROUP(TestMqttHumidifier)  // NOLINT
{
    

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

auto createMqttHumidifierMock(auto mqttDevMock)
{
    return MqttHumidifier<MqttDeviceMock>{mqttDevMock, "testName", "testId", "srv",
                                          123,         "user",     "pass"};
}

auto getMqttHumAndDevicePreparedForHandleMsg(auto mqttDevMock)
{
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mqttHumidifier.setRecvCallback(
        [](const std::string &msgType, uint8_t value)
        {
            mock("Lambda")
                .actualCall("callback")
                .withParameter("msgType", msgType.c_str())
                .withParameter("value", value);
        });

    return mqttHumidifier;
}

TEST(TestMqttHumidifier, UpdateWhenReconnectIsNotNeeded)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock").expectOneCall("update").andReturnValue(false);  // False -> not
                                                                           // reconnected
    mqttHumidifier.update();
}

TEST(TestMqttHumidifier, UpdateWhenReconnectIsNeeded)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock").expectOneCall("update").andReturnValue(true);  // True -> reconnected
    mock("MqttDeviceMock")
        .expectOneCall("subscribeTopic")
        .withParameter("topic", "humi/testId/humidifier/set");

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/humidifier/testId/humidifier/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/testId/humidity/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/testId/waterTank/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/testId/wifi/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/testId/light/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/testId/auto_mode/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/testId/night_mode/config")
        .ignoreOtherParameters();

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/availability")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/power/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidification_level/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidification_power/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidity/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/waterTank/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/light/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/auto_mode/state")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/night_mode/state")
        .ignoreOtherParameters();
    mqttHumidifier.update();
}

// // /********** Receiving mqtt tests **********/

TEST(TestMqttHumidifier, ShouldDiscardMsgIfIncomingDataIsWrong)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda").expectNoCall("callback");
    mqttDevMock->callRecvClbk("wrongTopic", "not json");
}

TEST(TestMqttHumidifier, ShouldHandleHumiditySetPointMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "humidification_level")
        .withParameter("value", 9);
    mqttDevMock->callRecvClbk("someTopic", R"({"humidification_level": 80})");
}

TEST(TestMqttHumidifier, ShouldHandleStateMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "power")
        .withParameter("value", 1);
    mqttDevMock->callRecvClbk("someTopic", R"({"power": "on"})");
}

TEST(TestMqttHumidifier, ShouldHandleModeMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "humidification_power")
        .withParameter("value", 1);
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidification_level/state")
        .ignoreOtherParameters();
    mqttDevMock->callRecvClbk("someTopic", R"({"humidification_power": "Normal"})");
}

TEST(TestMqttHumidifier, ShouldHandleAutoModeMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "auto_mode")
        .withParameter("value", 1);
    mqttDevMock->callRecvClbk("someTopic", R"({"auto_mode": 1})");
}

TEST(TestMqttHumidifier, ShouldHandleNightModeMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "night_mode")
        .withParameter("value", 1);
    mqttDevMock->callRecvClbk("someTopic", R"({"night_mode": 1})");
}

TEST(TestMqttHumidifier, ShouldHandleLightMsg)
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = getMqttHumAndDevicePreparedForHandleMsg(mqttDevMock);

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "light")
        .withParameter("value", 3);
    mqttDevMock->callRecvClbk("someTopic", R"({"light": 1})");
}

// /********** Publishing mqtt tests **********/

TEST(TestMqttHumidifier, ShouldPublishPowerMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "on")
        .ignoreOtherParameters();

    mqttHumidifier.publish("power", 1);
}

TEST(TestMqttHumidifier, ShouldPublishHumidityLvlMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "1")
        .ignoreOtherParameters();

    mqttHumidifier.publish("humidity_lvl", 1);
}

TEST(TestMqttHumidifier, ShouldPublishHumidificationPowerMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidification_power/state")
        .withParameter("data", "Normal")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humi/testId/humidification_level/state")
        .withParameter("data", "None")
        .ignoreOtherParameters();

    mqttHumidifier.publish("humidification_power", 1);
}

TEST(TestMqttHumidifier, ShouldPublishNightModeMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "1")
        .ignoreOtherParameters();

    mqttHumidifier.publish("night_mode", 1);
}

TEST(TestMqttHumidifier, ShouldPublishAutoModeMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "1")
        .ignoreOtherParameters();

    mqttHumidifier.publish("auto_mode", 1);
}

TEST(TestMqttHumidifier, ShouldPublishLightMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "1")
        .ignoreOtherParameters();

    mqttHumidifier.publish("light", 1);
}

TEST(TestMqttHumidifier, ShouldPublishWaterLvlMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    auto mqttHumidifier = createMqttHumidifierMock(mqttDevMock);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", "full")
        .ignoreOtherParameters();

    mqttHumidifier.publish("water_lvl", 1);
}
