#include "App.hpp"

#include <Arduino.h>
#include <fmt/format.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });

    WifiConfigurator::connectToWifi();
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

                // m_webPage.sendEvent("humidifierState", m_humidifierState.dump().c_str());
                m_webPage.sendEvent("humidifierState",
                                    fmt::format(R"({{"{}": {}}})", type, value).c_str());
                Logger::debug("Read message: {}, value {}", type, value);
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
