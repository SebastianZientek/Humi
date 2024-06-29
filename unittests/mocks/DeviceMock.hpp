#pragma once

#include <CppUTestExt/MockSupport.h>

class DeviceMock{
public:
    static auto millis()
    {
        return mock("DeviceMock").actualCall("millis").returnUnsignedLongIntValue();
    }
};