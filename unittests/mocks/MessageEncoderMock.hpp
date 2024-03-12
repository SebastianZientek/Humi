#include <CppUTestExt/MockSupport.h>

#include <cinttypes>
#include <vector>

class MessageEncoderMock
{
public:
    static std::vector<uint8_t> encode(const std::string &type, uint8_t value)
    {
        auto *returnVal = mock("MessageEncoderMock").actualCall("getData").returnPointerValue();
        return *static_cast<std::vector<uint8_t> *>(returnVal);
    }
};
