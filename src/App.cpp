#include "App.hpp"

#include <Arduino.h>
#include <fmt/format.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    Serial.begin(m_serialSpeed);
    Serial1.begin(m_serialSpeed);
    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });

    m_humidifierUart.sendMessage("wifi_indicator", 2);
    WifiConfigurator::connectToWifi();
    m_humidifierUart.sendMessage("wifi_indicator", 4);
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());

    auto onWebMsgClbk = [this](const std::string &msgType, uint8_t value)
    { return m_humidifierUart.sendMessage(msgType, value); };
    auto onEventInitClvk
        = [this]() { m_webPage.sendEvent("humidifierState", m_humidifierState.dump().c_str()); };

    m_webPage.start(onWebMsgClbk, onEventInitClvk);
    m_humidifierUart.setReceiveCallback(
        [this](const std::string &type, uint8_t value)
        {
            if (!type.empty())
            {
                m_humidifierState[type] = value;
                m_webPage.sendEvent("humidifierState",
                                    fmt::format(R"({{"{}": {}}})", type, value).c_str());
                Logger::debug("Read message: {}, value {}", type, value);
            }
            if (type == "wifi_pair")
            {
                m_humidifierUart.sendMessage("wifi_indicator", 0);
                m_webPage.stop();
                WifiConfigurator::startConfigPortal();
                m_humidifierUart.sendMessage("wifi_indicator", 3);
                ESP.restart();
            }
        });
}

void App::update()
{
    static constexpr auto uartUpdatePeriodMs = 250;
    static auto uartLastUpdate = millis();
    if (uartLastUpdate + uartUpdatePeriodMs < millis())
    {
        uartLastUpdate = millis();
        m_humidifierUart.update();
    }
}
