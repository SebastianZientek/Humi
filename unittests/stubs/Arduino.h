#pragma once

class SerialStub
{
public:
    void begin(size_t)
    {

    }

    template <typename... Ts>
    void print(Ts... args)
    {
    }

    template <typename... Ts>
    void println(Ts... args)
    {
    }

    template <typename... Ts>
    void printf(Ts... args)
    {
    }
};

SerialStub Serial;  // NOLINT
SerialStub Serial1;  // NOLINT