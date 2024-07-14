#pragma once

#include <concepts>
#include <functional>
#include <string>
#include <cstdint>

template <typename T>
concept CMqttDevice = requires(T mqtt,
    const std::string& deviceId,
    const std::string& srv,
    uint16_t port,
    const std::string& user,
    const std::string& pass,
    const std::string& lastWillTopic,
    const std::string& lastWillMsg,
    const std::string& topic,
    const std::string& data,
    std::function<void(const std::string&, const std::string&)> recvCallback)
{
    { mqtt.start(deviceId, srv, port, user, pass, lastWillTopic, lastWillMsg) } -> std::same_as<void>;
    { mqtt.update() } -> std::same_as<bool>;
    { mqtt.isConnected() } -> std::same_as<bool>;
    { mqtt.setRecvCallback(recvCallback) } -> std::same_as<void>;
    { mqtt.sendData(topic, data) } -> std::same_as<void>;
    { mqtt.subscribeTopic(topic) } -> std::same_as<void>;
};