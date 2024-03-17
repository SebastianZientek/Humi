#include "App.hpp"

#include <Arduino.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });

    WifiConfigurator::connectToWifi();
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());
    m_webPage.start([this](const std::string &msgType, uint8_t value)
                    { return m_humidifierUart.sendMessage(msgType, value); });

    m_humidifierUart.setReceiveCallback(
        [](const std::string &type, uint8_t value)
        {
            if (!type.empty())
            {
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
