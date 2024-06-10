#pragma once

#include <nlohmann/json.hpp>

template <typename Filesystem>
class Configuration
{
public:
    Configuration(Filesystem &filesystem)
        : m_fileSystem(filesystem)
    {
        m_data["mqttEnabled"] = false;
        m_data["mqttUser"] = "defaultUser";
        m_data["mqttPasswd"] = "defaultPasswd";
        m_data["mqttIP"] = "192.168.1.1";
        m_data["mqttPort"] = 1883;

        m_data["otaEnabled"] = false;
    }

    void load()
    {
        auto file = m_fileSystem.open(m_path, "r");
        if (!file)
        {
            Logger::error("Can't load configuration");
            return;
        }
        std::string data = file.readString().c_str();
        file.close();

        auto parsedData = nlohmann::json::parse(data, nullptr, false);
        if (parsedData.is_discarded())
        {
            Logger::error("Can't parse json data, {}", data);
            return;
        }

        m_data = parsedData;
    }

    void save()
    {
        auto file = m_fileSystem.open(m_path, "w");
        if (!file)
        {
            Logger::error("Can't save configuration");
            return;
        }
        file.print(m_data.dump().c_str());
        Logger::info("Saved {}", m_data.dump());

        file.close();
    }

    void setMqttEnabled(bool enabled)
    {
        m_data["mqttEnabled"] = enabled;
    }

    bool isMqttEnabled()
    {
        return m_data["mqttEnabled"];
    }

    void setMqttUser(const std::string &user)
    {
        m_data["mqttUser"] = user;
    }

    std::string getMqttUser()
    {
        return m_data["mqttUser"];
    }

    void setMqttPasswd(const std::string &passwd)
    {
        m_data["mqttPasswd"] = passwd;
    }

    std::string getMqttPasswd()
    {
        return m_data["mqttPasswd"];
    }

    void setMqttIP(const std::string &ip)
    {
        m_data["mqttIP"] = ip;
    }

    std::string getMqttIP()
    {
        return m_data["mqttIP"];
    }

    void setMqttPort(int port)
    {
        m_data["mqttPort"] = port;
    }

    int getMqttPort()
    {
        return m_data["mqttPort"];
    }

    void setOtaEnabled(bool enabled)
    {
        m_data["otaEnabled"] = enabled;
    }

    bool isOtaEnabled()
    {
        return m_data["otaEnabled"];
    }

    std::string dumpWithoutPasswd()
    {
        nlohmann::json dump;

        dump["mqttEnabled"] = m_data["mqttEnabled"];
        dump["mqttUser"] = m_data["mqttUser"];
        dump["mqttIP"] = m_data["mqttIP"];
        dump["mqttPort"] = m_data["mqttPort"];
        dump["otaEnabled"] = m_data["otaEnabled"];

        return dump.dump();
    }

private:
    const char *m_path = "/config.json";
    Filesystem &m_fileSystem;
    nlohmann::json m_data;
};
