#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>

// Message from humidifier to wifi module: preamble (85, 170) + message content + value + control
// sum (sum of values % 256)

class MsgWifiToHumidifier
{
public:
    MsgWifiToHumidifier(const std::string &type, uint8_t value)
    {
        if (m_msgContents.find(type) != m_msgContents.end())
        {
            // Preamble
            m_data.insert(m_data.end(), {85, 170});

            // Content
            auto &content = m_msgContents[type];
            m_data.insert(m_data.end(), content.begin(), content.end());

            // Value
            m_data.push_back(value);

            // Control sum
            uint8_t controlSum = std::accumulate(m_data.begin(), m_data.end(), 0) % 256;
            m_data.push_back(controlSum);
        }
    }
    std::vector<uint8_t> &getData()
    {
        return m_data;
    }

private:
    std::vector<uint8_t> m_data{};
    std::map<std::string, std::vector<uint8_t>> m_msgContents{
        {"power", {0, 6, 0, 5, 10, 1, 0, 1}},
        {"humidification_power", {0, 6, 0, 5, 106, 4, 0, 1}},
        {"humidification_level", {0, 6, 0, 5, 105, 4, 0, 1}},
        {"night_mode", {0, 6, 0, 5, 103, 1, 0, 1}},
        {"auto_mode", {0, 6, 0, 5, 104, 1, 0, 1}},
        {"light", {0, 6, 0, 5, 102, 4, 0, 1}},
        {"timer", {0, 6, 0, 5, 108, 4, 0, 1}},
        {"wifi_indicator", {0, 3, 0, 1}}};
};
