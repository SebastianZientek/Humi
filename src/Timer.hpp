#pragma once

#include <Arduino.h>

#include <functional>

template<typename Device>
class Timer
{
    using FunType = std::function<void()>;

public:
    void setCallback(const FunType &fun)
    {
        m_function = fun;
    }

    void start(std::size_t timeoutMillis, bool repeat = false)
    {
        m_period = timeoutMillis;
        m_startTime = Device::millis();

        m_stopped = false;
        m_repeat = repeat;
    }

    void stop()
    {
        m_stopped = true;
    }
    
    void update()
    {
        auto currentTime = Device::millis();
        if (!m_stopped && m_period + m_startTime <= currentTime)
        {
            m_function();

            if (m_repeat)
            {
                m_startTime = currentTime;
            }
            else
            {
                m_stopped = true;
            }
        }
    }

private:
    FunType m_function = [] {};
    bool m_stopped = false;
    bool m_repeat = false;
    std::size_t m_period = 0;
    std::size_t m_startTime = 0;
};
