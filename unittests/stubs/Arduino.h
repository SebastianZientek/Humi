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

extern SerialStub Serial;  // NOLINT
extern SerialStub Serial1;  // NOLINT
