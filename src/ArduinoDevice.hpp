#pragma once

#include <Arduino.h>

class ArduinoDevice
{
public:
    static auto millis()
    {
        return ::millis();
    }
};
