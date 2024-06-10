#include "App.hpp"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <fmt/format.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    Serial.begin(m_serialSpeed);
    Serial1.begin(m_serialSpeed);

    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });
    if (!LittleFS.begin())
    {
        Logger::error("Can't initialize LittleFS");
    }

    m_config.load();

    m_humidifierUart.sendMessage("wifi_indicator", 2);
    WifiConfigurator::connectToWifi();
    m_humidifierUart.sendMessage("wifi_indicator", 4);
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());

    auto onWebMsgClbk = [this](const std::string &msgType, uint8_t value)
    { return m_humidifierUart.sendMessage(msgType, value); };
    auto onEventClbk = [this]()
    {
        m_webPage.sendEvent("humidifierState", m_humidifierState.dump().c_str());
        m_webPage.sendEvent("config", m_config.dumpWithoutPasswd().c_str());
    };
    auto onMqttConfigClbk = [this](bool enabled, const std::string &user, const std::string &passwd,
                                   const std::string &ip, int port)
    {
        m_config.setMqttEnabled(enabled);
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

    m_webPage.start(onWebMsgClbk, onEventClbk, onMqttConfigClbk, onOtaConfigClbk);
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

    setupMqtt();

    if (m_config.isOtaEnabled())
    {
        ArduinoOTA.begin();
    }
}

void App::update()
{
    static constexpr auto otaUpdatePeriodMs = 10000;
    static auto otaLastUpdate = millis();
    if (otaLastUpdate + otaUpdatePeriodMs < millis())
    {
        otaLastUpdate = millis();
        ArduinoOTA.handle();
    }

    if (m_config.isOtaEnabled())
    {
        static constexpr auto uartUpdatePeriodMs = 250;
        static auto uartLastUpdate = millis();
        if (uartLastUpdate + uartUpdatePeriodMs < millis())
        {
            uartLastUpdate = millis();
            m_humidifierUart.update();
        }
    }

    static constexpr auto wifiStateUpdatePeriodMs = 1000;
    static auto wifiStateLastUpdate = millis();
    if (wifiStateLastUpdate + wifiStateUpdatePeriodMs < millis())
    {
        wifiStateLastUpdate = millis();
        if (m_config.isMqttEnabled())
        {
            if (m_mqttHumidifier->isConnected())
            {
                m_webPage.sendEvent("mqttState", "connected"); // Should be updated only on change or more rarely
                m_mqttHumidifier->publishWifi(WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(),
                                              WiFi.RSSI());
            }
            else {
                m_webPage.sendEvent("mqttState", "disconnected");
            }

            m_mqttHumidifier->update();
        }
        else {
            m_webPage.sendEvent("mqttState", "disabled");
        }
    }

    if (m_shouldRestart)
    {
        ESP.restart();
    }
}

void App::setupMqtt()
{
    if (m_config.isMqttEnabled() == false)
    {
        Logger::info("Mqtt disabled");
        return;
    }

    auto mqtt_server = m_config.getMqttIP();
    auto mqtt_port = m_config.getMqttPort();
    auto mqtt_username = m_config.getMqttUser();
    auto mqtt_password = m_config.getMqttPasswd();

    m_mqttAdp->start(std::to_string(ESP.getChipId()), mqtt_server, mqtt_port, mqtt_username,
                     mqtt_password);
    m_mqttHumidifier
        = std::make_shared<MqttHumidifier<MqttAdp>>(m_mqttAdp, "SXHumidifier", ESP.getChipId());
    m_mqttHumidifier->setRecvCallback(
        [this](const std::string &msgType, uint8_t value)
        {
            Logger::debug("MQTT READ VALUES: {}, value {}", msgType, value);
            m_humidifierUart.sendMessage(msgType, value);
        });

    m_mqttHumidifier->publishActive(true);
}
