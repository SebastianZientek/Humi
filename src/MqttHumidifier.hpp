#include <charconv>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

#include "Logger.hpp"
#include "CMqttDevice.hpp"

template <CMqttDevice MqttDevice>
class MqttHumidifier
{
public:
    using RecvClbk = std::function<void(const std::string &msgType, uint8_t value)>;

    MqttHumidifier(std::shared_ptr<MqttDevice> mqttDevice,
                   const std::string &name,
                   const std::string &deviceId)
        : m_mqttDevice(mqttDevice)
        , m_name(name)
        , m_chipId(deviceId)
        , m_baseUri("humidifier/humi_" + m_chipId)
        , MQTT_AVAILABILITY(m_baseUri + "/status")
        , MQTT_STATE(m_baseUri + "/state")
        , MQTT_COMMAND(m_baseUri + "/command")
        , MQTT_AUTOCONF_HUMIDITY_SENSOR("homeassistant/sensor/" + m_baseUri + "_humidity/config")
        , MQTT_AUTOCONF_WIFI_SENSOR("homeassistant/sensor/" + m_baseUri + "_wifi/config")
        , MQTT_AUTOCONF_WATER_TANK_SENSOR("homeassistant/sensor/" + m_baseUri
                                          + "_water_tank/config")
        , MQTT_AUTOCONF_LIGHT_SWITCH("homeassistant/switch/" + m_baseUri + "_light/config")
        , MQTT_AUTOCONF_AUTOMODE_SWITCH("homeassistant/switch/" + m_baseUri + "_auto_mode/config")
        , MQTT_AUTOCONF_NIGHTMODE_SWITCH("homeassistant/switch/" + m_baseUri + "_night_mode/config")
        , MQTT_AUTOCONF_HUMIDIFIER("homeassistant/humidifier/" + m_baseUri + "_humidifier/config")

    {
    }

    void update()
    {
        auto reconnected = m_mqttDevice->update();
        if (reconnected)
        {
            m_mqttDevice->subscribeTopic(MQTT_COMMAND);
            publishAutoConfig();
            m_mqttDevice->sendData(MQTT_AVAILABILITY, "online");
        }
    }

    void setRecvCallback(const RecvClbk &clbk)
    {
        m_mqttDevice->setRecvCallback(
            [clbk](const std::string &topic, const std::string &data)
            {
                Logger::debug("MQTT READ: {}, value {}", topic, data);
                auto message = nlohmann::json::parse(data, nullptr, false);

                if (message.is_discarded())
                {
                    Logger::error("Can't parse mqtt data, {}", data);
                }
                else if (message.contains("humiditySetpoint"))
                {
                    uint8_t value = message["humiditySetpoint"];
                    clbk("humidification_level", (value - 35) / 5);
                }
                else if (message.contains("state"))
                {
                    std::string state = message["state"];
                    uint8_t value = state == "on" ? 1 : 0;
                    clbk("power", value);
                }
                else if (message.contains("mode"))
                {
                    std::string mode = message["mode"];

                    uint8_t value{};
                    auto [ptr, ec] = std::from_chars(mode.data(), mode.data() + mode.size(), value);
                    if (ec == std::errc() && value >= 1 && value <= 4)
                    {
                        clbk("humidification_power", value - 1);
                    }
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
                else if (message.contains("light"))
                {
                    std::string state = message["light"];
                    uint8_t value = state == "on" ? 3 : 0;
                    clbk("light", value);
                }
            });
    }

    void publishMqtt(const std::string &type, uint8_t value)
    {
        if (type == "power")
        {
            publishActive(value == 1);
        }
        else if (type == "humidification_level" && value > 0)
        {
            constexpr auto humidificationStep = 5;
            constexpr auto minHumidification = 40;

            // To keep value between 40% and 75% (value = 1 is 40%)
            publishTargetHumidity((value - 1) * humidificationStep + minHumidification);
        }
        else if (type == "humidity_lvl")
        {
            publishSensorHumidity(value);
        }
        else if (type == "humidification_power")
        {
            publishMode(std::to_string(value + 1));
        }
        else if (type == "night_mode")
        {
            publishNightMode(value);
        }
        else if (type == "auto_mode")
        {
            publishAutoMode(value);
        }
        else if (type == "light")
        {
            publishLight(value != 0);
        }
        else if (type == "water_lvl")
        {
            publishWaterTank(value == 1);
        }
    }

    void publishActive(bool active)
    {
        nlohmann::json state;
        state["state"] = active ? "on" : "off";
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishWifi(const std::string &ssid, const std::string &ip, int8_t rssi)
    {
        nlohmann::json state;
        nlohmann::json wifi;

        wifi["ssid"] = ssid;
        wifi["ip"] = ip;
        wifi["rssi"] = rssi;

        state["wifi"] = wifi;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

private:
    std::shared_ptr<MqttDevice> m_mqttDevice{};
    RecvClbk m_clbk;

    std::string m_name;
    std::string m_chipId;
    std::string m_baseUri;

    std::string MQTT_AVAILABILITY;
    std::string MQTT_STATE;
    std::string MQTT_COMMAND;
    std::string MQTT_AUTOCONF_HUMIDITY_SENSOR;
    std::string MQTT_AUTOCONF_WIFI_SENSOR;
    std::string MQTT_AUTOCONF_WATER_TANK_SENSOR;
    std::string MQTT_AUTOCONF_LIGHT_SWITCH;
    std::string MQTT_AUTOCONF_AUTOMODE_SWITCH;
    std::string MQTT_AUTOCONF_NIGHTMODE_SWITCH;
    std::string MQTT_AUTOCONF_HUMIDIFIER;

    void publishAutoConfig()
    {
        nlohmann::json device;
        std::string identifier = m_name;

        device["identifiers"] = {identifier};
        device["manufacturer"] = "Custom";
        device["model"] = "Humi Custom Wifi Module";
        device["name"] = identifier;
        device["sw_version"] = __DATE__;

        sendAutoConfHumidity(device, identifier);
        sendAutoConfWifiSignal(device, identifier);
        sendAutoConfWaterTank(device, identifier);
        sendAutoConfigLight(device, identifier);
        sendAutoConfigAutoMode(device, identifier);
        sendAutoConfigNightMode(device, identifier);
        sendAutoConfigHumidifyingPower(device, identifier);
    }

    void sendAutoConfHumidity(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "Humidity";
        autoconf["device_class"] = "humidity";
        autoconf["unit_of_measurement"] = "%";
        autoconf["value_template"] = "{{value_json.humidity}}";
        autoconf["unique_id"] = identifier + "_humidity";

        m_mqttDevice->sendData(MQTT_AUTOCONF_HUMIDITY_SENSOR, autoconf.dump());
    }

    void sendAutoConfWifiSignal(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "WiFi";
        autoconf["value_template"] = "{{value_json.wifi.rssi}}";
        autoconf["unique_id"] = identifier + "_wifi";
        autoconf["unit_of_measurement"] = "dBm";
        autoconf["json_attributes_topic"] = MQTT_STATE;
        autoconf["json_attributes_template"]
            = "{\"ssid\": \"{{value_json.wifi.ssid}}\", \"ip\": "
              "\"{{value_json.wifi.ip}}\"}";
        autoconf["icon"] = "mdi:wifi";
        autoconf["entity_category"] = "diagnostic";

        m_mqttDevice->sendData(MQTT_AUTOCONF_WIFI_SENSOR, autoconf.dump());
    }

    void sendAutoConfWaterTank(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "Water Tank";
        autoconf["value_template"] = "{{value_json.waterTank}}";
        autoconf["unique_id"] = identifier + "_water_tank";
        autoconf["icon"] = "mdi:cup-water";
        autoconf["entity_category"] = "diagnostic";

        m_mqttDevice->sendData(MQTT_AUTOCONF_WATER_TANK_SENSOR, autoconf.dump());
    }

    void sendAutoConfigLight(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["mode_state_topic"] = MQTT_STATE;
        autoconf["mode_command_topic"] = MQTT_COMMAND;
        autoconf["mode_state_template"] = "{{value_json.mode}}";
        autoconf["mode_command_template"] = R"({"mode": "{{value}}"})";
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["command_topic"] = MQTT_COMMAND;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "Light";
        autoconf["value_template"] = "{{value_json.light}}";
        autoconf["unique_id"] = identifier + "_light";
        autoconf["payload_on"] = R"({"light": "on"})";
        autoconf["payload_off"] = R"({"light": "off"})";
        autoconf["state_on"] = "on";
        autoconf["state_off"] = "off";
        autoconf["icon"] = "mdi:lightbulb";
        autoconf["entity_category"] = "config";

        m_mqttDevice->sendData(MQTT_AUTOCONF_LIGHT_SWITCH, autoconf.dump());
    }

    void sendAutoConfigAutoMode(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["mode_state_topic"] = MQTT_STATE;
        autoconf["mode_command_topic"] = MQTT_COMMAND;
        autoconf["mode_state_template"] = "{{value_json.mode}}";
        autoconf["mode_command_template"] = R"({"mode": "{{value}}"})";
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["command_topic"] = MQTT_COMMAND;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "Auto";
        autoconf["value_template"] = "{{value_json.auto_mode}}";
        autoconf["unique_id"] = identifier + "_auto_mode";
        autoconf["payload_on"] = R"({"auto_mode": 1})";
        autoconf["payload_off"] = R"({"auto_mode": 0})";
        autoconf["state_on"] = 1;
        autoconf["state_off"] = 0;
        autoconf["icon"] = "mdi:auto-mode";
        autoconf["entity_category"] = "config";

        m_mqttDevice->sendData(MQTT_AUTOCONF_AUTOMODE_SWITCH, autoconf.dump());
    }

    void sendAutoConfigNightMode(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["mode_state_topic"] = MQTT_STATE;
        autoconf["mode_command_topic"] = MQTT_COMMAND;
        autoconf["mode_state_template"] = "{{value_json.mode}}";
        autoconf["mode_command_template"] = R"({"mode": "{{value}}"})";
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["command_topic"] = MQTT_COMMAND;
        autoconf["state_topic"] = MQTT_STATE;
        autoconf["name"] = "Night mode";
        autoconf["value_template"] = "{{value_json.night_mode}}";
        autoconf["unique_id"] = identifier + "_night_mode";
        autoconf["payload_on"] = R"({"night_mode": 1})";
        autoconf["payload_off"] = R"({"night_mode": 0})";
        autoconf["state_on"] = 1;
        autoconf["state_off"] = 0;
        autoconf["icon"] = "mdi:weather-night";
        autoconf["entity_category"] = "config";

        m_mqttDevice->sendData(MQTT_AUTOCONF_NIGHTMODE_SWITCH, autoconf.dump());
    }

    void sendAutoConfigHumidifyingPower(const nlohmann::json &device, const std::string &identifier)
    {
        nlohmann::json autoconf;

        autoconf["device"] = device;
        autoconf["availability_topic"] = MQTT_AVAILABILITY;
        autoconf["name"] = "Humidifier";
        autoconf["unique_id"] = identifier + "_humidifier";
        autoconf["device_class"] = "humidifier";

        autoconf["max_humidity"] = 75;
        autoconf["min_humidity"] = 40;

        autoconf["state_topic"] = MQTT_STATE;
        autoconf["state_value_template"] = R"({"state": "{{value_json.state}}"})";
        autoconf["payload_on"] = R"({"state": "on"})";
        autoconf["payload_off"] = R"({"state": "off"})";
        autoconf["command_topic"] = MQTT_COMMAND;
        autoconf["state_value_template"] = R"({"state": "{{value_json.state}}"})";

        nlohmann::json modes;
        modes.push_back("1");
        modes.push_back("2");
        modes.push_back("3");
        modes.push_back("4");

        autoconf["modes"] = modes;
        autoconf["target_humidity_state_topic"] = MQTT_STATE;
        autoconf["target_humidity_command_topic"] = MQTT_COMMAND;
        autoconf["target_humidity_state_template"] = "{{value_json.humiditySetpoint | int}}";
        autoconf["target_humidity_command_template"] = "{\"humiditySetpoint\":{{value | int}}}";
        autoconf["mode_state_topic"] = MQTT_STATE;
        autoconf["mode_command_topic"] = MQTT_COMMAND;
        autoconf["mode_state_template"] = "{{value_json.mode}}";
        autoconf["mode_command_template"] = R"({"mode": "{{value}}"})";

        m_mqttDevice->sendData(MQTT_AUTOCONF_HUMIDIFIER, autoconf.dump());
    }

    void publishTargetHumidity(int targetHumidity)
    {
        nlohmann::json state;
        state["humiditySetpoint"] = targetHumidity;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishSensorHumidity(int humidity)
    {
        nlohmann::json state;
        state["humidity"] = humidity;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishMode(std::string mode)
    {
        nlohmann::json state;
        state["mode"] = mode;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishAutoMode(uint8_t enable)
    {
        nlohmann::json state;
        state["auto_mode"] = enable;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishNightMode(uint8_t enable)
    {
        nlohmann::json state;
        state["night_mode"] = enable;
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishLight(bool enable)
    {
        nlohmann::json state;
        state["light"] = enable ? "on" : "off";
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }

    void publishWaterTank(bool full)
    {
        nlohmann::json state;
        state["waterTank"] = full ? "full" : "empty";
        m_mqttDevice->sendData(MQTT_STATE, state.dump());
    }
};
