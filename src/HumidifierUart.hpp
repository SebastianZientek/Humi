#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "CUart.hpp"
#include "CMessageEncoder.hpp"

template <CUart Uart, CMessageEncoder MsgEncoder>
class HumidifierUart
{
public:
    explicit HumidifierUart(Uart *uart)
        : m_uart(uart)
    {
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

private:
    Uart *m_uart;
};
