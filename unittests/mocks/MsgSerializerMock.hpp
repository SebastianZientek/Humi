#include <CppUTestExt/MockSupport.h>

#include <cinttypes>

class MsgSerializerMock
{
public:
    MsgSerializerMock(const std::string &type, uint8_t value){};

    std::vector<uint8_t> &getData()
    {
        auto *returnVal = mock("MsgSerializerMock").actualCall("getData").returnPointerValue();
        return *static_cast<std::vector<uint8_t> *>(returnVal);
    }
};
