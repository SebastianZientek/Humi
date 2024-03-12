#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "CUart.hpp"
#include "CMsgSerializer.hpp"

template <CUart Uart, CMsgSerializer MsgSerializer>
class HumidifierUart
{
public:
    explicit HumidifierUart(Uart *uart)
        : m_uart(uart)
    {
    }

    bool sendMessage(const std::string &msgType, uint8_t value)
    {
        MsgSerializer msg(msgType, value);
        auto &data = msg.getData();

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
