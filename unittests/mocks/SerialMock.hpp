#include <CppUTestExt/MockSupport.h>

#include <cinttypes>
#include <deque>

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

    int available()
    {
        return m_buffer.size();
    }

    int read()
    {
        if (m_buffer.size() > 0)
        {
            auto byte = m_buffer.front();
            m_buffer.pop_front();
            return byte;
        }

        return -1;
    }

    // Testability

    void setBuffer(const std::deque<uint8_t> &data)
    {
        m_buffer = data;
    }

private:
    std::deque<uint8_t> m_buffer;
};
