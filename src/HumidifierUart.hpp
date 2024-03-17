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

        auto positionToRead = 0;
        while (true)
        {
            std::span<uint8_t> bufferRangeToRead{m_buffer.begin() + positionToRead, m_buffer.end()};
            auto [msg, consumedBytes] = MsgEncoder::decodeFirsMsgInRange(bufferRangeToRead);

            // Can't consume data
            if (consumedBytes == 0)
            {
                break;
            }
            m_onReceiveClbk(msg.type, msg.value);

            // Consumed everything
            if (consumedBytes == m_buffer.size())
            {
                break;
            }

            positionToRead += consumedBytes;
        }

        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + positionToRead);
    }

private:
    constexpr static auto m_bufferSize = 100;

    Uart *m_uart;
    std::vector<uint8_t> m_buffer;
    OnReceiveClbk m_onReceiveClbk = [](const std::string &, uint8_t) {};

    void readDataToBuffer()
    {
        while (m_uart->available() && m_buffer.size() < m_bufferSize)
        {
            auto byte = m_uart->read();
            m_buffer.push_back(byte);
        }
    }
};
