#include <CppUTestExt/MockSupport.h>

#include <cinttypes>

class SerialMock
{
public:
    size_t write(const uint8_t *buffer, size_t size)
    {
        return mock("SerialMock")
            .actualCall("write")
            .withParameter("size", static_cast<int>(size))
            .returnUnsignedIntValue();
    }
};
