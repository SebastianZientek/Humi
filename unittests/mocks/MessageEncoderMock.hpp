#include <CppUTestExt/MockSupport.h>

#include <cinttypes>
#include <vector>
#include <algorithm>
#include <span>

class SpanComparator : public MockNamedValueComparator
{
public:
    bool isEqual(const void* object1, const void* object2) override
    {
        auto span1 = *static_cast<const std::span<uint8_t>*>(object1);
        auto span2 = *static_cast<const std::span<uint8_t>*>(object2);
        return std::equal(span1.begin(), span1.end(), span2.begin(), span2.end());
    }

    SimpleString valueToString(const void* object) override
    {
        return StringFrom(object);
    }
};

class MessageEncoderMock
{
public:
    struct Msg
    {
        std::string type;
        uint8_t value;
    };

    static std::vector<uint8_t> encode(const std::string &type, uint8_t value)
    {
        auto *returnVal = mock("MessageEncoderMock").actualCall("getData").returnPointerValue();
        return *static_cast<std::vector<uint8_t> *>(returnVal);
    }

    static std::pair<Msg, size_t> decodeFirsMsgInRange(std::span<uint8_t> buffer)
    {
        auto *returnVal = mock("MessageEncoderMock").actualCall("decodeFirsMsgInRange").withParameterOfType("span", "buffer", &buffer).returnPointerValue();
        return *static_cast<std::pair<Msg, size_t> *>(returnVal);
    }
};
