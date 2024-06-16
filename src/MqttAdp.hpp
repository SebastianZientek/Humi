
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <cstdint>
#include <functional>
#include <string>

class MqttAdp
{
public:
    using RecvClbk = std::function<void(const std::string &topic, const std::string &datau)>;

    void start(const std::string &deviceId,
               const std::string &srv,
               uint16_t port,
               const std::string &user,
               const std::string &pass);
    bool update();
    bool isConnected();
    void setRecvCallback(const RecvClbk &clbk);
    void sendData(const std::string &topic, const std::string &data);
    void subscribeTopic(const std::string &topic);

private:
    constexpr static auto mqttBufferSize = 1200;

    std::string m_deviceId{};
    std::string m_srv{};
    std::string m_user{};
    std::string m_pass{};

    bool m_enabled{false};
    RecvClbk m_recvClbk{};

    WiFiClient m_client{};
    PubSubClient m_mqtt{m_client};
};
