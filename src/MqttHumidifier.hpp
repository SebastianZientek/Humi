#include <charconv>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "CMqttDevice.hpp"
#include "Logger.hpp"
#undef B1
#include <fmt/core.h>

template <CMqttDevice MqttDevice>
class MqttHumidifier
{
public:
    using RecvClbk = std::function<void(const std::string &msgType, uint8_t value)>;

    MqttHumidifier(std::shared_ptr<MqttDevice> mqttDevice,
                   const std::string &name,
                   const std::string &deviceId,
                   const std::string &mqttServer,
                   uint16_t mqttPort,
                   const std::string &mqttUsername,
                   const std::string &mqttPassword)
        : m_mqttDevice(mqttDevice)
        , m_name(name)
        , m_deviceId(deviceId)
        , m_mqttServer(mqttServer)
        , m_mqttPort(mqttPort)
        , m_mqttUsername(mqttUsername)
        , m_mqttPassword(mqttPassword)
    {
        TOPIC_COMMON = fmt::format("humi/{}", deviceId);
        TOPIC_COMMAND = fmt::format("{}/humidifier/set", TOPIC_COMMON);

        TOPIC_AUTOCONFIG_HUMIDIFIER
            = fmt::format("homeassistant/humidifier/{}/humidifier/config", deviceId);
        TOPIC_AUTOCONFIG_WATER_TANK
            = fmt::format("homeassistant/sensor/{}/waterTank/config", deviceId);
        TOPIC_AUTOCONFIG_HUMIDITY
            = fmt::format("homeassistant/sensor/{}/humidity/config", deviceId);
        TOPIC_LIGHT_AUTOCONFIG = fmt::format("homeassistant/switch/{}/light/config", deviceId);
        TOPIC_AUTOMODE_AUTOCONFIG
            = fmt::format("homeassistant/switch/{}/auto_mode/config", deviceId);
        TOPIC_NIGHTMODE_AUTOCONFIG
            = fmt::format("homeassistant/switch/{}/night_mode/config", deviceId);
        TOPIC_WIFI_AUTOCONFIG = fmt::format("homeassistant/sensor/{}/wifi/config", deviceId);
    }

    void start()
    {
        m_mqttDevice->start(m_deviceId, m_mqttServer, m_mqttPort, m_mqttUsername, m_mqttPassword,
                            fmt::format("{}/availability", TOPIC_COMMON), "offline");
    }

    void update()
    {
        auto reconnected = m_mqttDevice->update();
        if (reconnected)
        {
            m_mqttDevice->subscribeTopic(TOPIC_COMMAND);
            sendAutoconfig();
            prepareAfterReconnection();
        }
    }

    void setRecvCallback(const RecvClbk &clbk)
    {
        m_mqttDevice->setRecvCallback(
            [this, clbk](const std::string &topic, const std::string &data)
            {
                Logger::debug("Recv: {} -> {}", topic, data);
                auto message = nlohmann::json::parse(data, nullptr, false);

                if (message.is_discarded())
                {
                    Logger::error("Can't parse mqtt data, {}", data);
                }
                else if (message.contains("power"))
                {
                    std::string state = message["power"];
                    clbk("power", state == "on" ? 1 : 0);
                }
                else if (message.contains("humidification_level"))
                {
                    uint8_t value = message["humidification_level"];
                    clbk("humidification_level", (value - 35) / 5);
                }
                else if (message.contains("humidification_power"))
                {
                    std::string mode = message["humidification_power"];
                    static std::map<std::string, int> modeToValue{
                        {"Low", 0}, {"Normal", 1}, {"High", 2}, {"Turbo", 3}, {"Target", 4}};
                    uint8_t value = modeToValue[mode];

                    if (value >= 0 && value < 4)
                    {
                        clbk("humidification_power", value);
                        m_mqttDevice->sendData(
                            fmt::format("{}/humidification_level/state", TOPIC_COMMON), "None");
                    }
                    else if (value == 4)
                    {
                        clbk("humidification_level", 2);
                    }
                }
                else if (message.contains("light"))
                {
                    uint8_t value = message["light"];
                    clbk("light", value != 0 ? 3 : 0);
                }
                else if (message.contains("auto_mode"))
                {
                    uint8_t value = message["auto_mode"];
                    clbk("auto_mode", value);
                }
                else if (message.contains("night_mode"))
                {
                    uint8_t value = message["night_mode"];
                    clbk("night_mode", value);
                }
            });
    }

    void publish(const std::string &type, uint8_t value)
    {
        if (type == "power")
        {
            m_mqttDevice->sendData(fmt::format("{}/power/state", TOPIC_COMMON),
                                   value == 1 ? "on" : "off");
        }
        else if (type == "humidification_level" && value > 0)
        {
            constexpr auto humidificationStep = 5;
            constexpr auto minHumidification = 40;

            // To keep value between 40% and 75% (value = 1 is 40%)
            auto targetHumidity = (value - 1) * humidificationStep + minHumidification;

            m_mqttDevice->sendData(fmt::format("{}/humidification_level/state", TOPIC_COMMON),
                                   std::to_string(targetHumidity));
            m_mqttDevice->sendData(fmt::format("{}/humidification_power/state", TOPIC_COMMON),
                                   "Target");
        }
        else if (type == "humidification_power")
        {
            static std::array<std::string, 5> mode{"Low", "Normal", "High", "Turbo", "Target"};

            if (value >= 0 && value < 5)
            {
                m_mqttDevice->sendData(fmt::format("{}/humidification_power/state", TOPIC_COMMON),
                                       mode[value]);
                m_mqttDevice->sendData(fmt::format("{}/humidification_level/state", TOPIC_COMMON),
                                       "None");
            }
        }
        else if (type == "humidity_lvl")
        {
            m_mqttDevice->sendData(fmt::format("{}/humidity/state", TOPIC_COMMON),
                                   std::to_string(value));
        }
        else if (type == "water_lvl")
        {
            m_mqttDevice->sendData(fmt::format("{}/waterTank/state", TOPIC_COMMON),
                                   value == 1 ? "full" : "empty");
        }
        else if (type == "light")
        {
            m_mqttDevice->sendData(fmt::format("{}/light/state", TOPIC_COMMON),
                                   std::to_string(value != 0 ? 1 : 0));
        }
        else if (type == "auto_mode")
        {
            m_mqttDevice->sendData(fmt::format("{}/auto_mode/state", TOPIC_COMMON),
                                   std::to_string(value));
        }
        else if (type == "night_mode")
        {
            m_mqttDevice->sendData(fmt::format("{}/night_mode/state", TOPIC_COMMON),
                                   std::to_string(value));
        }
    }

    void publishWifi(const std::string &ssid, const std::string &ip, int8_t rssi)
    {
        nlohmann::json wifi = {{"ssid", ssid}, {"ip", ip}, {"rssi", rssi}};
        m_mqttDevice->sendData(fmt::format("{}/wifi/state", TOPIC_COMMON), wifi.dump());
    }

private:
    std::shared_ptr<MqttDevice> m_mqttDevice{};

    std::string m_name;
    std::string m_deviceId;

    std::string m_mqttServer;
    uint16_t m_mqttPort;
    std::string m_mqttUsername;
    std::string m_mqttPassword;
    std::string TOPIC_COMMON;
    std::string TOPIC_COMMAND;
    std::string TOPIC_AUTOCONFIG_HUMIDIFIER;
    std::string TOPIC_AUTOCONFIG_WATER_TANK;
    std::string TOPIC_AUTOCONFIG_HUMIDITY;
    std::string TOPIC_LIGHT_AUTOCONFIG;
    std::string TOPIC_AUTOMODE_AUTOCONFIG;
    std::string TOPIC_NIGHTMODE_AUTOCONFIG;
    std::string TOPIC_WIFI_AUTOCONFIG;

    void prepareAfterReconnection()
    {
        m_mqttDevice->sendData(fmt::format("{}/availability", TOPIC_COMMON), "online");
        m_mqttDevice->sendData(fmt::format("{}/power/state", TOPIC_COMMON), "on");
        m_mqttDevice->sendData(fmt::format("{}/humidification_level/state", TOPIC_COMMON), "None");
        m_mqttDevice->sendData(fmt::format("{}/humidification_power/state", TOPIC_COMMON), "None");
        m_mqttDevice->sendData(fmt::format("{}/humidity/state", TOPIC_COMMON), "Unknown");
        m_mqttDevice->sendData(fmt::format("{}/waterTank/state", TOPIC_COMMON), "full");
        m_mqttDevice->sendData(fmt::format("{}/light/state", TOPIC_COMMON), "0");
        m_mqttDevice->sendData(fmt::format("{}/auto_mode/state", TOPIC_COMMON), "0");
        m_mqttDevice->sendData(fmt::format("{}/night_mode/state", TOPIC_COMMON), "0");
    }

    void sendAutoconfig()
    {
        nlohmann::json device = {{"identifiers", {m_name}},
                                 {"manufacturer", "Custom"},
                                 {"model", "Humi Custom Wifi Module"},
                                 {"name", m_name}};

        sendAutoConfigHumidifier(device);
        sendAutoConfHumidity(device);
        sendAutoConfWaterTank(device);
        sendAutoConfWifi(device);
        sendAutoConfigLight(device);
        sendAutoConfigAutoMode(device);
        sendAutoConfigNightMode(device);
    }

    void sendAutoConfigHumidifier(const nlohmann::json &device)
    {
        nlohmann::json humidifierAutoConfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"name", "Humidifier"},
               {"unique_id", fmt::format("humidifier_{}", m_deviceId)},
               {"device_class", "humidifier"},
               {"max_humidity", 75},
               {"min_humidity", 40},
               {"payload_off", "off"},
               {"payload_on", "on"},
               {"state_topic", fmt::format("{}/power/state", TOPIC_COMMON)},
               {"command_topic", TOPIC_COMMAND},
               {"command_template", R"({"power": "{{value}}"})"},
               {"modes", {"Low", "Normal", "High", "Turbo", "Target"}},
               {"target_humidity_state_topic",
                fmt::format("{}/humidification_level/state", TOPIC_COMMON)},
               {"target_humidity_state_template", "{{value}}"},
               {"target_humidity_command_topic", TOPIC_COMMAND},
               {"target_humidity_command_template", R"({"humidification_level": {{value}}})"},
               {"mode_state_topic", fmt::format("{}/humidification_power/state", TOPIC_COMMON)},
               {"mode_state_template", "{{value}}"},
               {"mode_command_topic", TOPIC_COMMAND},
               {"mode_command_template", R"({"humidification_power": "{{value}}"})"}};

        m_mqttDevice->sendData(TOPIC_AUTOCONFIG_HUMIDIFIER, humidifierAutoConfig.dump());
    }

    void sendAutoConfHumidity(const nlohmann::json &device)
    {
        nlohmann::json humidityAutoconfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"state_topic", fmt::format("{}/humidity/state", TOPIC_COMMON)},
               {"name", "Humidity"},
               {"device_class", "humidity"},
               {"unit_of_measurement", "%"},
               {"value_template", "{{value}}"},
               {"unique_id", fmt::format("humidity_{}", m_deviceId)}};

        m_mqttDevice->sendData(TOPIC_AUTOCONFIG_HUMIDITY, humidityAutoconfig.dump());
    }

    void sendAutoConfWaterTank(const nlohmann::json &device)
    {
        nlohmann::json waterTankAutoConfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"state_topic", fmt::format("{}/waterTank/state", TOPIC_COMMON)},
               {"name", "Water Tank"},
               {"unique_id", fmt::format("waterTank_{}", m_deviceId)},
               {"icon", "mdi:cup-water"}};

        m_mqttDevice->sendData(TOPIC_AUTOCONFIG_WATER_TANK, waterTankAutoConfig.dump());
    }

    void sendAutoConfWifi(const nlohmann::json &device)
    {
        nlohmann::json wifiAutoconfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"state_topic", fmt::format("{}/wifi/state", TOPIC_COMMON)},
               {"name", "WiFi"},
               {"value_template", "{{value_json.rssi}}"},
               {"unique_id", fmt::format("wifi_{}", m_deviceId)},
               {"unit_of_measurement", "dBm"},
               {"json_attributes_topic", fmt::format("{}/wifi/attr", TOPIC_COMMON)},
               {"json_attributes_template",
                R"({"ssid": "{{value_json.ssid}}", "ip": "{{value_json.ip}}"})"},
               {"icon", "mdi:wifi"}};

        m_mqttDevice->sendData(TOPIC_WIFI_AUTOCONFIG, wifiAutoconfig.dump());
    }

    void sendAutoConfigLight(const nlohmann::json &device)
    {
        nlohmann::json lightAutoconfig
            = {{"device", device},
               {"device_class", "switch"},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"state_topic", fmt::format("{}/light/state", TOPIC_COMMON)},
               {"command_topic", TOPIC_COMMAND},
               {"name", "Light"},
               {"unique_id", fmt::format("light_{}", m_deviceId)},
               {"state_off", 0},
               {"state_on", 1},
               {"payload_off", R"({"light": 0})"},
               {"payload_on", R"({"light": 1})"},
               {"icon", "mdi:lightbulb"},
               {"entity_category", "config"}};

        m_mqttDevice->sendData(TOPIC_LIGHT_AUTOCONFIG, lightAutoconfig.dump());
    }

    void sendAutoConfigAutoMode(const nlohmann::json &device)
    {
        nlohmann::json autoModeAutoconfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"command_topic", TOPIC_COMMAND},
               {"state_topic", fmt::format("{}/auto_mode/state", TOPIC_COMMON)},
               {"name", "Auto"},
               {"unique_id", fmt::format("auto_mode_{}", m_deviceId)},
               {"state_off", 0},
               {"state_on", 1},
               {"payload_off", R"({"auto_mode": 0})"},
               {"payload_on", R"({"auto_mode": 1})"},
               {"icon", "mdi:auto-mode"},
               {"entity_category", "config"}};
        m_mqttDevice->sendData(TOPIC_AUTOMODE_AUTOCONFIG, autoModeAutoconfig.dump());
    }

    void sendAutoConfigNightMode(const nlohmann::json &device)
    {
        nlohmann::json nightModeAutoconfig
            = {{"device", device},
               {"availability_topic", fmt::format("{}/availability", TOPIC_COMMON)},
               {"command_topic", TOPIC_COMMAND},
               {"state_topic", fmt::format("{}/night_mode/state", TOPIC_COMMON)},
               {"name", "Night mode"},
               {"unique_id", fmt::format("night_mode_{}", m_deviceId)},
               {"state_off", 0},
               {"state_on", 1},
               {"payload_off", R"({"night_mode": 0})"},
               {"payload_on", R"({"night_mode": 1})"},
               {"icon", "mdi:weather-night"},
               {"entity_category", "config"}};
        m_mqttDevice->sendData(TOPIC_NIGHTMODE_AUTOCONFIG, nightModeAutoconfig.dump());
    }
};
