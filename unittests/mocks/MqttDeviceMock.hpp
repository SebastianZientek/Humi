#pragma once

#include <CppUTestExt/MockSupport.h>

#include <cstdint>
#include <functional>
#include <string>

class MqttDeviceMock
{
public:
    using RecvClbk = std::function<void(const std::string &topic, const std::string &data)>;

    void start(const std::string &deviceId,
               const std::string &srv,
               uint16_t port,
               const std::string &user,
               const std::string &pass)
    {
        mock("MqttDeviceMock")
            .actualCall("start")
            .withParameter("deviceId", deviceId.c_str())
            .withParameter("port", port)
            .withParameter("user", user.c_str())
            .withParameter("pass", pass.c_str());
    }

    bool update()
    {
        return mock("MqttDeviceMock").actualCall("update").returnBoolValue();
    }

    bool isConnected()
    {
        return mock("MqttDeviceMock").actualCall("isConnected").returnBoolValue();
    }

    void setRecvCallback(const RecvClbk &clbk)
    {
        m_recvClbk = clbk;
    }

    void sendData(const std::string &topic, const std::string &data)
    {
        mock("MqttDeviceMock")
            .actualCall("sendData")
            .withParameter("topic", topic.c_str())
            .withParameter("data", data.c_str());
    }

    void subscribeTopic(const std::string &topic)
    {
        mock("MqttDeviceMock").actualCall("subscribeTopic").withParameter("topic", topic.c_str());
    }

    // Testability functions

    void callRecvClbk(const std::string &topic, const std::string &data)
    {
        m_recvClbk(topic, data);
    }

private:
    RecvClbk m_recvClbk;
};
