#pragma once

#include <Arduino.h>

#include <functional>

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
        m_startTime = millis();

        m_stopped = false;
        m_repeat = repeat;
    }

    void stop()
    {
        m_stopped = true;
    }
    
    void update()
    {
        if (!m_stopped && m_period + m_startTime <= millis())
        {
            m_function();

            if (m_repeat)
            {
                m_startTime = millis();
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
