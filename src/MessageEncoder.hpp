#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>

// Message from humidifier to wifi module: preamble (85, 170) + message content + value + control
// sum (sum of values % 256)

class MessageEncoder
{
public:
    struct Msg
    {
        std::string type;
        uint8_t value;
    };

    static std::pair<Msg, size_t> decodeFirsMsgInRange(std::span<uint8_t> buffer);
    static std::vector<uint8_t> encode(const std::string &type, uint8_t value);
};
