#include "MqttAdp.hpp"

#include <algorithm>
#include <iterator>

#include "Logger.hpp"

void MqttAdp::start(const std::string &deviceId,
                    const std::string &srv,
                    uint16_t port,
                    const std::string &user,
                    const std::string &pass)
{
    // Have to save srv address as PubSubClient keeps only pointer to data
    m_deviceId = deviceId;
    m_srv = srv;
    m_user = user;
    m_pass = pass;

    m_mqtt.setServer(m_srv.c_str(), port);
    m_mqtt.setKeepAlive(10);
    m_mqtt.setBufferSize(mqttBufferSize);
}

bool MqttAdp::update()
{
    auto reconnected = false;
    if (!m_mqtt.connected())
    {
        reconnected = m_mqtt.connect(m_deviceId.c_str(), m_user.c_str(), m_pass.c_str());
    }
    m_mqtt.loop();

    return reconnected;
}

bool MqttAdp::isConnected()
{
    return m_mqtt.connected();
}

void MqttAdp::setRecvCallback(const RecvClbk &clbk)
{
    m_recvClbk = clbk;

    auto callbackFun = [this](char *topic, byte *inData, unsigned int size)
    {
        std::string strData{};
        std::copy(inData, &inData[size], std::back_inserter(strData));

        m_recvClbk(topic, strData);
    };

    m_mqtt.setCallback(callbackFun);
}

void MqttAdp::sendData(const std::string &topic, const std::string &data)
{
    if (!m_mqtt.connected())
    {
        // Logger::warning("MqttAdp::sendData not connected");
        return;
    }

    if (mqttBufferSize < data.size())
    {
        Logger::error("Too small mqtt buffer, buffer size: {}, data size: {}", mqttBufferSize,
                      data.size());
        return;
    }
    m_mqtt.publish(topic.c_str(), data.c_str(), true);
}

void MqttAdp::subscribeTopic(const std::string &topic)
{
    m_mqtt.subscribe(topic.c_str());
}
