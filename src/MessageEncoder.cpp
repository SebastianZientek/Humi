#include "MessageEncoder.hpp"

#include <map>
#include <numeric>

const std::map<std::string, std::vector<uint8_t>> encodingMap{
    {"power", {0, 6, 0, 5, 10, 1, 0, 1}},
    {"humidification_power", {0, 6, 0, 5, 106, 4, 0, 1}},
    {"humidification_level", {0, 6, 0, 5, 105, 4, 0, 1}},
    {"night_mode", {0, 6, 0, 5, 103, 1, 0, 1}},
    {"auto_mode", {0, 6, 0, 5, 104, 1, 0, 1}},
    {"light", {0, 6, 0, 5, 102, 4, 0, 1}},
    {"timer", {0, 6, 0, 5, 108, 4, 0, 1}},
    {"wifi_indicator", {0, 3, 0, 1}}};

std::vector<uint8_t> MessageEncoder::encode(const std::string &type, uint8_t value)
{
    std::vector<uint8_t> data;
    if (encodingMap.find(type) != encodingMap.end())
    {
        // Preamble
        data.insert(data.end(), {85, 170});

        // Content
        const auto &content = encodingMap.at(type);
        data.insert(data.end(), content.begin(), content.end());

        // Value
        data.push_back(value);

        // Control sum
        uint8_t controlSum = std::accumulate(data.begin(), data.end(), 0) % 256;
        data.push_back(controlSum);
    }
    return data;
}
