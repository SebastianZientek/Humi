#pragma once

#include <Arduino.h>

#include <memory>
#include <nlohmann/json.hpp>

#include "HumidifierUart.hpp"
#include "MessageEncoder.hpp"
#include "Resources.hpp"
#include "WebPage.hpp"
#include "WebServer/EventSrcClient.hpp"
#include "WebServer/WebRequest.hpp"
#include "WebServer/WebServer.hpp"
#include "MqttAdp.hpp"
#include "MqttHumidifier.hpp"
#include "Configuration.hpp"
#include <LittleFS.h>

class App
{
public:
    void init();
    void update();

private:
    void setupMqtt();

    using WebSrv = WebServer<WebRequest, EventSrcClient>;
    using MainWebPage = WebPage<WebSrv, Resources>;
    constexpr static auto m_serialSpeed = 9600;

    std::shared_ptr<WebSrv> m_webSrv{std::make_shared<WebSrv>(80)};
    MainWebPage m_webPage{m_webSrv};
    HumidifierUart<decltype(Serial), MessageEncoder> m_humidifierUart{&Serial};
    nlohmann::json m_humidifierState;
    std::shared_ptr<MqttAdp> m_mqttAdp{std::make_shared<MqttAdp>()};
    std::shared_ptr<MqttHumidifier<MqttAdp>> m_mqttHumidifier;
    Configuration<FS> m_config{LittleFS};
    bool m_shouldRestart{false};
};
