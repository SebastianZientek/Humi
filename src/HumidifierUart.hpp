#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <vector>

#include "CMessageEncoder.hpp"
#include "CUart.hpp"

template <CUart Uart, CMessageEncoder MsgEncoder>
class HumidifierUart
{
public:
    using OnReceiveClbk = std::function<void(const std::string &type, uint8_t value)>;

    explicit HumidifierUart(Uart *uart)
        : m_uart(uart)
    {
        m_buffer.reserve(m_bufferSize);
    }

    bool sendMessage(const std::string &msgType, uint8_t value)
    {
        auto data = MsgEncoder::encode(msgType, value);

        if (data.empty())
        {
            return false;
        }

        m_uart->write(data.data(), data.size());
        return true;
    }

    void setReceiveCallback(const OnReceiveClbk &clbk)
    {
        m_onReceiveClbk = clbk;
    }

    void update()
    {
        readDataToBuffer();

        size_t positionToRead = 0;
        while (true)
        {
            std::span<uint8_t> bufferRangeToRead{m_buffer.begin() + positionToRead,  // NOLINT
                                                 m_buffer.end()};
            auto [msg, consumedBytes] = MsgEncoder::decodeFirsMsgInRange(bufferRangeToRead);

            // Can't consume data
            if (consumedBytes == 0)
            {
                break;
            }
            m_onReceiveClbk(msg.type, msg.value);

            positionToRead += consumedBytes;
            // There is nothing to read
            if (positionToRead >= m_buffer.size())
            {
                break;
            }
        }

        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + positionToRead);  // NOLINT
    }

private:
    constexpr static auto m_bufferSize = 128;

    Uart *m_uart;
    std::vector<uint8_t> m_buffer;
    OnReceiveClbk m_onReceiveClbk = [](const std::string &, uint8_t) {};

    void readDataToBuffer()
    {
        while (m_uart->available() > 0 && m_buffer.size() < m_bufferSize)
        {
            auto byte = m_uart->read();
            m_buffer.push_back(byte);
        }
    }
};
