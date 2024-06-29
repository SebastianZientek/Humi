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
    setupDefaultState();
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
    m_mqttHumidifier->update();

    if (m_shouldRestart)
    {
        Logger::info("Reboot");
        delay(1000);
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

void App::setupDefaultState()
{
    m_humidifierState["humidification_level"] = 1;
    m_humidifierState["power"] = 1;
    m_humidifierState["auto_mode"] = 0;
    m_humidifierState["night_mode"] = 0;
    m_humidifierState["light"] = 0;
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
                    m_mqttHumidifier->publishMqtt(type, value);
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

    auto mqttDeviceId = std::to_string(ESP.getChipId());
    auto mqttDeviceName = m_config.getMqttName();
    auto mqttServer = m_config.getMqttIP();
    auto mqttPort = m_config.getMqttPort();
    auto mqttUsername = m_config.getMqttUser();
    auto mqttPassword = m_config.getMqttPasswd();

    if (m_config.isMqttEnabled())
    {
        m_mqttAdp->start(mqttDeviceId, mqttServer, mqttPort, mqttUsername, mqttPassword);
    }
    else {
        Logger::info("Mqtt disabled");
    }

    m_mqttHumidifier
        = std::make_shared<MqttHumidifier<MqttAdp>>(m_mqttAdp, mqttDeviceName, mqttDeviceId);
    m_mqttHumidifier->setRecvCallback(
        [this](const std::string &msgType, uint8_t value)
        {
            Logger::debug("MQTT READ VALUES: {}, value {}", msgType, value);
            m_humidifierUart.sendMessage(msgType, value);
        });

    m_mqttHumidifier->publishActive(true);


    // send initial data
    for (auto it = m_humidifierState.begin(); it != m_humidifierState.end(); ++it)
    {
        std::string key = it.key();
        uint8_t value = it.value();
        Logger::debug("MQTT Initial send: {} = {}", key, value);
        m_mqttHumidifier->publishMqtt(key, value);
    }
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
            auto mqttEnabled = m_config.isMqttEnabled();
            auto mqttState = mqttEnabled == false ? "disabled" : (m_mqttAdp->isConnected() ? "connected" : "disconnected");
            m_webPage.sendEvent("mqttState", mqttState);

            m_mqttHumidifier->publishWifi(WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(),
                                          WiFi.RSSI());
        });

    m_stateUpdateTimer.start(10000, true);
}
