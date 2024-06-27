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

auto getMqttHumAndDevicePreparedForHandleMsg()
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mqttHumidifier.setRecvCallback(
        [](const std::string &msgType, uint8_t value)
        {
            mock("Lambda")
                .actualCall("callback")
                .withParameter("msgType", msgType.c_str())
                .withParameter("value", value);
        });

    return std::make_pair(mqttHumidifier, mqttDevMock);
}

TEST(TestMqttHumidifier, UpdateWhenReconnectIsNotNeeded)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock").expectOneCall("update").andReturnValue(false);  // False -> not
                                                                           // reconnected
    mqttHumidifier.update();
}

TEST(TestMqttHumidifier, UpdateWhenReconnectIsNeeded)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock").expectOneCall("update").andReturnValue(true);  // True -> reconnected
    mock("MqttDeviceMock")
        .expectOneCall("subscribeTopic")
        .withParameter("topic", "humidifier/humi_testId/command");

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/humidifier/humi_testId_humidity/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/humidifier/humi_testId_wifi/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/sensor/humidifier/humi_testId_water_tank/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/humidifier/humi_testId_light/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/humidifier/humi_testId_auto_mode/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/switch/humidifier/humi_testId_night_mode/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "homeassistant/humidifier/humidifier/humi_testId_humidifier/config")
        .ignoreOtherParameters();
    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("topic", "humidifier/humi_testId/status")
        .ignoreOtherParameters();
    mqttHumidifier.update();
}

/********** Receiving mqtt tests **********/

TEST(TestMqttHumidifier, ShouldDiscardMsgIfIncomingDataIsWrong)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda").expectNoCall("callback");
    mqttDevMock->callRecvClbk("wrongTopic", "not json");
}

TEST(TestMqttHumidifier, ShouldHandleHumiditySetPointMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "humidification_level")
        .withParameter("value", 9);
    mqttDevMock->callRecvClbk("someTopic", R"({"humiditySetpoint": 80})");
}

TEST(TestMqttHumidifier, ShouldHandleStateMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "power")
        .withParameter("value", 1);
    mqttDevMock->callRecvClbk("someTopic", R"({"state": "on"})");
}

TEST(TestMqttHumidifier, ShouldHandleModeMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "humidification_power")
        .withParameter("value", 1);
    // Looks confising but mqtt have different values for the same states (to be precise state - 1)
    mqttDevMock->callRecvClbk("someTopic", R"({"mode": "2"})");
}

TEST(TestMqttHumidifier, ShouldHandleAutoModeMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "auto_mode")
        .withParameter("value", 2);
    mqttDevMock->callRecvClbk("someTopic", R"({"auto_mode": 2})");
}

TEST(TestMqttHumidifier, ShouldHandleNightModeMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "night_mode")
        .withParameter("value", 1);
    mqttDevMock->callRecvClbk("someTopic", R"({"night_mode": 1})");
}

TEST(TestMqttHumidifier, ShouldHandleLightMsg)
{
    auto [mqttHumidifier, mqttDevMock] = getMqttHumAndDevicePreparedForHandleMsg();

    mock("Lambda")
        .expectOneCall("callback")
        .withParameter("msgType", "light")
        .withParameter("value", 3);
    mqttDevMock->callRecvClbk("someTopic", R"({"light": "on"})");
}

/********** Publishing mqtt tests **********/

TEST(TestMqttHumidifier, ShouldPublishPowerMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"state":"on"})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("power", 1);
}

TEST(TestMqttHumidifier, ShouldPublishHumidityLvlMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"humidity":1})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("humidity_lvl", 1);
}

TEST(TestMqttHumidifier, ShouldPublishHumidificationPowerMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"mode":"2"})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("humidification_power", 1);
}

TEST(TestMqttHumidifier, ShouldPublishNightModeMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"night_mode":1})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("night_mode", 1);
}

TEST(TestMqttHumidifier, ShouldPublishAutoModeMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"auto_mode":1})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("auto_mode", 1);
}

TEST(TestMqttHumidifier, ShouldPublishLightMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"light":"on"})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("light", 1);
}

TEST(TestMqttHumidifier, ShouldPublishWaterLvlMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"waterTank":"full"})")
        .ignoreOtherParameters();

    mqttHumidifier.publishMqtt("water_lvl", 1);
}

TEST(TestMqttHumidifier, ShouldPublishActiveStateMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"state":"on"})")
        .ignoreOtherParameters();
    mqttHumidifier.publishActive(true);

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", R"({"state":"off"})")
        .ignoreOtherParameters();
    mqttHumidifier.publishActive(false);
}

TEST(TestMqttHumidifier, ShouldPublishWifiStateMsg)  // NOLINT
{
    auto mqttDevMock = std::make_shared<MqttDeviceMock>();
    MqttHumidifier<MqttDeviceMock> mqttHumidifier{mqttDevMock, "testName", "testId"};

    nlohmann::json state;
    nlohmann::json wifi;

    wifi["ssid"] = "ssid";
    wifi["ip"] = "ip";
    wifi["rssi"] = 123;

    state["wifi"] = wifi;
    std::string data = state.dump();

    mock("MqttDeviceMock")
        .expectOneCall("sendData")
        .withParameter("data", data.c_str())
        .ignoreOtherParameters();
    mqttHumidifier.publishWifi("ssid", "ip", 123);
}
