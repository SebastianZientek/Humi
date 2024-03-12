#pragma once

#include <cstdint>
#include <vector>
#include <string>

// Message from humidifier to wifi module: preamble (85, 170) + message content + value + control
// sum (sum of values % 256)

class MessageEncoder
{
public:
    static std::vector<uint8_t> encode(const std::string &type, uint8_t value);
};
