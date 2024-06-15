#include "App.hpp"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <fmt/format.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    setupPeripherals();

    m_config.load();
    setupWifi();
    setupWebPage();
    setupHumidifierUart();
    setupMqtt();

    if (m_config.isOtaEnabled())
    {
        ArduinoOTA.begin();
    }

    setupTimers();
}

void App::update()
{
    m_humidifierUartTimer.update();
    m_otaTimer.update();
    m_stateUpdateTimer.update();

    if (m_config.isMqttEnabled())
    {
        m_mqttHumidifier->update();
    }

    if (m_shouldRestart)
    {
        ESP.restart();
    }
}

void App::setupPeripherals()
{
    Serial.begin(m_serialSpeed);
    Serial1.begin(m_serialSpeed);

    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });
    if (!LittleFS.begin())
    {
        Logger::error("Can't initialize LittleFS");
    }
}

void App::setupWifi()
{
    Logger::debug("Setup WiFi");
    m_humidifierUart.sendMessage("wifi_indicator", 2);
    WifiConfigurator::connectToWifi();
    m_humidifierUart.sendMessage("wifi_indicator", 4);
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());
}

void App::setupWebPage()
{
    Logger::debug("Setup WebPage");
    auto onWebMsgClbk = [this](const std::string &msgType, uint8_t value)
    { return m_humidifierUart.sendMessage(msgType, value); };
    auto onWebEventClbk = [this]()
    {
        m_webPage.sendEvent("humidifierState", m_humidifierState.dump().c_str());
        m_webPage.sendEvent("config", m_config.dumpWithoutPasswd().c_str());
    };
    auto onMqttConfigClbk = [this](bool enabled, const std::string &name, const std::string &user,
                                   const std::string &passwd, const std::string &ip, int port)
    {
        m_config.setMqttEnabled(enabled);
        m_config.setMqttName(name);
        m_config.setMqttUser(user);
        m_config.setMqttPasswd(passwd);
        m_config.setMqttIP(ip);
        m_config.setMqttPort(port);
        m_config.save();

        m_shouldRestart = true;
    };
    auto onOtaConfigClbk = [this](bool enabled)
    {
        m_config.setOtaEnabled(enabled);
        m_config.save();

        m_shouldRestart = true;
    };

    m_webPage.start(onWebMsgClbk, onWebEventClbk, onMqttConfigClbk, onOtaConfigClbk);
}

void App::setupHumidifierUart()
{
    Logger::debug("Setup HumidifierUart");
    m_humidifierUart.setReceiveCallback(
        [this](const std::string &type, uint8_t value)
        {
            if (!type.empty())
            {
                if (m_humidifierState[type] != value)
                {
                    m_humidifierState[type] = value;
                    m_webPage.sendEvent("humidifierState",
                                        fmt::format(R"({{"{}": {}}})", type, value).c_str());
                    if (m_config.isMqttEnabled())
                    {
                        m_mqttHumidifier->publishMqtt(type, value);
                    }
                    Logger::debug("Read message: {}, value {}", type, value);
                }
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

void App::setupMqtt()
{
    Logger::debug("Setup Mqtt");
    if (m_config.isMqttEnabled() == false)
    {
        Logger::info("Mqtt disabled");
        return;
    }

    auto mqttDeviceName = m_config.getMqttName();
    auto mqttServer = m_config.getMqttIP();
    auto mqttPort = m_config.getMqttPort();
    auto mqttUsername = m_config.getMqttUser();
    auto mqttPassword = m_config.getMqttPasswd();

    m_mqttAdp->start(std::to_string(ESP.getChipId()), mqttServer, mqttPort, mqttUsername,
                     mqttPassword);
    m_mqttHumidifier
        = std::make_shared<MqttHumidifier<MqttAdp>>(m_mqttAdp, mqttDeviceName, ESP.getChipId());
    m_mqttHumidifier->setRecvCallback(
        [this](const std::string &msgType, uint8_t value)
        {
            Logger::debug("MQTT READ VALUES: {}, value {}", msgType, value);
            m_humidifierUart.sendMessage(msgType, value);
        });

    m_mqttHumidifier->publishActive(true);
}

void App::setupTimers()
{
    Logger::debug("Setup Timers");
    m_humidifierUartTimer.setCallback([this] { m_humidifierUart.update(); });
    m_humidifierUartTimer.start(250, true);

    m_otaTimer.setCallback(
        [this]
        {
            if (m_config.isOtaEnabled())
            {
                ArduinoOTA.handle();
            }
        });
    m_otaTimer.start(5000, true);

    m_humidifierUartTimer.setCallback([this] { m_humidifierUart.update(); });
    m_humidifierUartTimer.start(250, true);

    m_stateUpdateTimer.setCallback(
        [this]
        {
            if (m_config.isMqttEnabled())
            {
                if (m_mqttHumidifier->isConnected())
                {
                    m_webPage.sendEvent("mqttState", "connected");
                }
                else
                {
                    m_webPage.sendEvent("mqttState", "disconnected");
                }

                m_mqttHumidifier->update();
            }
            else
            {
                m_webPage.sendEvent("mqttState", "disabled");
            }

            m_mqttHumidifier->publishWifi(WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(),
                                          WiFi.RSSI());
        });

    m_stateUpdateTimer.start(10000, true);
}
