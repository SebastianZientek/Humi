#pragma once

class DeviceMock{
public:
    static auto millis()
    {
        return mock("DeviceMock").actualCall("millis").returnUnsignedLongIntValue();
    }
};