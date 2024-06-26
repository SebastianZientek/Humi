#include "MessageEncoder.hpp"

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>

namespace
{
constexpr auto preambleSize = 2;
constexpr auto headerSize = 6;
constexpr auto valueSize = 1;
constexpr auto checkSumSize = 1;
constexpr auto payloadSizeByteIndex = 5;

const std::map<std::string, std::vector<uint8_t>> encodingMap{
    {"power", {0, 6, 0, 5, 10, 1, 0, 1}},
    {"humidification_power", {0, 6, 0, 5, 106, 4, 0, 1}},
    {"humidification_level", {0, 6, 0, 5, 105, 4, 0, 1}},
    {"night_mode", {0, 6, 0, 5, 103, 1, 0, 1}},
    {"auto_mode", {0, 6, 0, 5, 104, 1, 0, 1}},
    {"light", {0, 6, 0, 5, 102, 4, 0, 1}},
    {"timer", {0, 6, 0, 5, 108, 4, 0, 1}},
    {"wifi_indicator", {0, 3, 0, 1}}};

const std::map<std::vector<uint8_t>, std::string> decodingMap{
    {{0, 0, 0}, "i_am_alive"},
    {{3, 4, 0}, "wifi_pair"},
    {{3, 7, 0, 5, 10, 1, 0, 1}, "power"},
    {{3, 7, 0, 5, 106, 4, 0, 1}, "humidification_power"},
    {{3, 7, 0, 5, 105, 4, 0, 1}, "humidification_level"},
    {{3, 7, 0, 5, 103, 1, 0, 1}, "night_mode"},
    {{3, 7, 0, 5, 104, 1, 0, 1}, "auto_mode"},
    {{3, 7, 0, 5, 102, 4, 0, 1}, "light"},
    {{3, 7, 0, 5, 108, 4, 0, 1}, "timer"},
    {{3, 7, 0, 5, 101, 4, 0, 1}, "water_lvl"},
    {{3, 7, 0, 8, 109, 2, 0, 4, 0, 0, 0}, "humidity_lvl"}};

auto findPreamble(std::span<uint8_t> dataRange)
{
    auto preamble = std::array<uint8_t, 2>{85, 170};  // NOLINT
    return std::search(dataRange.begin(), dataRange.end(), preamble.begin(), preamble.end());
}

template <typename It>
size_t getPayloadSize(It msgStartIt)
{
    auto playloadSizeIt = msgStartIt + payloadSizeByteIndex;
    return *playloadSizeIt;
}

template <typename It>
std::optional<It> getMessageEnd(It msgStartIt, It dataEnd)
{
    auto payloadSize = getPayloadSize(msgStartIt);

    size_t msgLength = headerSize + payloadSize + checkSumSize;
    size_t amountOfDataInBuffer = std::distance(msgStartIt, dataEnd);

    if (msgLength > amountOfDataInBuffer)
    {
        return std::nullopt;
    }

    return msgStartIt + msgLength;
}

std::span<uint8_t> getMessageRange(const std::span<uint8_t> &dataRange)
{
    std::pair<MessageEncoder::Msg, size_t> noMessageDecoded = {{}, 0};
    auto msgStartIt = findPreamble(dataRange);
    if (msgStartIt == dataRange.end())
    {
        // If there are no messages within the specified range, the function should return the end
        // of the range to skip the contents of the buffer.
        return {dataRange.end(), dataRange.end()};
    }

    auto msgEnd = getMessageEnd(msgStartIt, dataRange.end());
    if (!msgEnd.has_value())
    {
        // If there is no end of the message, the function should return the position where the
        // message preamble starts. This allows skipping the data in the buffer that precedes the
        // message.
        return {msgStartIt, msgStartIt};
    }

    auto msgEndIt = msgEnd.value();
    return std::span<uint8_t>{msgStartIt, msgEndIt};
}

std::string getMsgType(const std::span<uint8_t> &msg)
{
    // auto startMsgTypeIt = msg.begin() + headerSize;
    auto startMsgTypeIt = msg.begin() + preambleSize;
    auto endMsgTypeIt = msg.end() - valueSize - checkSumSize;

    if (startMsgTypeIt > endMsgTypeIt)
    {
        startMsgTypeIt = endMsgTypeIt;
    }

    std::vector<uint8_t> msgTypeData(startMsgTypeIt, endMsgTypeIt);
    if (decodingMap.find(msgTypeData) == decodingMap.end())
    {
        return "";
    }

    return decodingMap.at(msgTypeData);
}

uint8_t getValue(const std::span<uint8_t> &msg)
{
    return *(msg.end() - valueSize - checkSumSize);
}

bool isChecksumOk(const std::span<uint8_t> &msg)
{
    auto checkSumValue = *(msg.end() - valueSize);
    auto computedCheckSum = std::accumulate(msg.begin(), msg.end() - 1, 0) % 256;  // NOLINT

    return checkSumValue == computedCheckSum;
}
}  // namespace

std::pair<MessageEncoder::Msg, size_t> MessageEncoder::decodeFirsMsgInRange(
    std::span<uint8_t> buffer)
{
    auto msg = getMessageRange(buffer);
    size_t readedBytes = std::distance(buffer.begin(), msg.end());

    if (msg.empty())
    {
        return {{}, readedBytes};
    }

    if (!isChecksumOk(msg))
    {
        return {{}, readedBytes};
    }

    const auto msgType = getMsgType(msg);
    if (msgType.empty())
    {
        return {{}, readedBytes};
    }

    return {{msgType, getValue(msg)}, readedBytes};
}

std::vector<uint8_t> MessageEncoder::encode(const std::string &type, uint8_t value)
{
    std::vector<uint8_t> data;
    if (encodingMap.find(type) != encodingMap.end())
    {
        // Preamble
        data.insert(data.end(), {85, 170});  // NOLINT

        // Content
        const auto &content = encodingMap.at(type);
        data.insert(data.end(), content.begin(), content.end());

        // Value
        data.push_back(value);

        // Control sum
        uint8_t controlSum = std::accumulate(data.begin(), data.end(), 0) % 256;  // NOLINT
        data.push_back(controlSum);
    }
    return data;
}
