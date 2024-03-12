#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "HumidifierUart.hpp"
#include "mocks/SerialMock.hpp"
#include "mocks/MessageEncoderMock.hpp"

// clang-format off
TEST_GROUP(TestHumidifierUart)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

TEST(TestHumidifierUart, ShouldSendMessageForCorrectType)  // NOLINT
{
    SerialMock serial;
    HumidifierUart<SerialMock, MessageEncoderMock> humidifierUart(&serial);

    std::vector<uint8_t> someData{1, 2, 3};
    mock("SerialMock").expectOneCall("write").ignoreOtherParameters().andReturnValue(1);
    mock("MessageEncoderMock").expectOneCall("getData").andReturnValue(&someData);

    auto sended = humidifierUart.sendMessage("timer", 1);
    CHECK_TRUE(sended);
}

TEST(TestHumidifierUart, ShouldntSendWrongMessage)  // NOLINT
{
    SerialMock serial;
    HumidifierUart<SerialMock, MessageEncoderMock> humidifierUart(&serial);

    std::vector<uint8_t> emptyData{};
    mock("SerialMock").expectNoCall("write");
    mock("MessageEncoderMock").expectOneCall("getData").andReturnValue(&emptyData);

    auto sended = humidifierUart.sendMessage("not_message_type", 1);
    CHECK_FALSE(sended);
}

