#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <span>

#include "HumidifierUart.hpp"
#include "mocks/MessageEncoderMock.hpp"
#include "mocks/SerialMock.hpp"

// clang-format off
TEST_GROUP(TestHumidifierUart)  // NOLINT
{
    void setup() override
    {
        static SpanComparator comparator;
        mock("MessageEncoderMock").installComparator("span", comparator);
    }

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

TEST(TestHumidifierUart, CallClbkWithDecodedOneMessage)  // NOLINT
{
    SerialMock serial;
    HumidifierUart<SerialMock, MessageEncoderMock> humidifierUart(&serial);
    humidifierUart.setReceiveCallback([](const std::string &type, uint8_t value)
                                      { mock("Lambda").actualCall("receiveDataClbk"); });
    constexpr auto msgType = "someType";
    constexpr auto value = 0;
    constexpr auto consumedBytes = 4;

    std::pair<MessageEncoderMock::Msg, size_t> outputFromDecoder{{msgType, value}, consumedBytes};
    std::vector<uint8_t> expMsgData{1, 1, 1, 1};
    std::span<uint8_t> expMsgDataSpan{expMsgData};

    mock("Lambda").expectOneCall("receiveDataClbk");
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan)
        .andReturnValue(&outputFromDecoder);

    serial.setBuffer({1, 1, 1, 1});
    humidifierUart.update();
}

TEST(TestHumidifierUart, CallClbkWithDecodedOneMessageAndExtraDataAtEnd)  // NOLINT
{
    SerialMock serial;
    HumidifierUart<SerialMock, MessageEncoderMock> humidifierUart(&serial);
    humidifierUart.setReceiveCallback([](const std::string &type, uint8_t value)
                                      { mock("Lambda").actualCall("receiveDataClbk"); });
    constexpr auto msgType = "someType";
    constexpr auto value = 0;
    constexpr auto consumedBytes = 4;
    std::pair<MessageEncoderMock::Msg, size_t> outputFromDecoder{{msgType, value}, consumedBytes};
    std::pair<MessageEncoderMock::Msg, size_t> emptyMessage{{}, 0};

    std::vector<uint8_t> expMsgData1{1, 1, 1, 1, 2, 2, 2};
    std::span<uint8_t> expMsgDataSpan1{expMsgData1};

    std::vector<uint8_t> expMsgData2{2, 2, 2};
    std::span<uint8_t> expMsgDataSpan2{expMsgData2};

    mock("Lambda").expectOneCall("receiveDataClbk");
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan1)
        .andReturnValue(&outputFromDecoder);
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan2)
        .andReturnValue(&emptyMessage);

    serial.setBuffer({1, 1, 1, 1, 2, 2, 2});
    humidifierUart.update();
}

TEST(TestHumidifierUart, CallClbkWithDecodedMoreMessagesAndExtraDataAtEnd)  // NOLINT
{
    SerialMock serial;
    HumidifierUart<SerialMock, MessageEncoderMock> humidifierUart(&serial);
    humidifierUart.setReceiveCallback([](const std::string &type, uint8_t value)
                                      { mock("Lambda").actualCall("receiveDataClbk"); });

    constexpr auto msgType = "someType";
    constexpr auto value = 0;
    constexpr auto consumedBytesFirstMsg = 4;
    constexpr auto consumedBytesSecondMsg = 3;
    std::pair<MessageEncoderMock::Msg, size_t> firstOutputFromDecoder{{msgType, value},
                                                                      consumedBytesFirstMsg};
    std::pair<MessageEncoderMock::Msg, size_t> secondOutputFromDecoder{{msgType, value},
                                                                       consumedBytesSecondMsg};
    std::pair<MessageEncoderMock::Msg, size_t> emptyMessage{{}, 0};

    std::vector<uint8_t> expMsgData1{1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3};
    std::span<uint8_t> expMsgDataSpan1{expMsgData1};

    std::vector<uint8_t> expMsgData2{2, 2, 2, 3, 3, 3, 3, 3};
    std::span<uint8_t> expMsgDataSpan2{expMsgData2};

    std::vector<uint8_t> expMsgData3{3, 3, 3, 3, 3};
    std::span<uint8_t> expMsgDataSpan3{expMsgData3};

    mock("Lambda").expectNCalls(2, "receiveDataClbk");
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan1)
        .andReturnValue(&firstOutputFromDecoder);
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan2)
        .andReturnValue(&secondOutputFromDecoder);
    mock("MessageEncoderMock")
        .expectOneCall("decodeFirsMsgInRange")
        .withParameterOfType("span", "buffer", &expMsgDataSpan3)
        .andReturnValue(&emptyMessage);

    serial.setBuffer({1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3});
    humidifierUart.update();
}
