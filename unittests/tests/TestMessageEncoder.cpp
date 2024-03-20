#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "MessageEncoder.hpp"

// clang-format off
TEST_GROUP(TestMessageEncoder)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

TEST(TestMessageEncoder, ShouldPrepareMessageIfTypeIsCorrect)  // NOLINT
{
    std::vector<uint8_t> expectedContent{85, 170, 0, 6, 0, 5, 106, 4, 0, 1, 2, 123};
    auto encoded = MessageEncoder::encode("humidification_power", 2);

    CHECK_TRUE(encoded == expectedContent);
}

TEST(TestMessageEncoder, ShouldReturnEmptyDataOnWrongMsgType)  // NOLINT
{
    std::vector<uint8_t> expectedContent{85, 170, 0, 6, 0, 5, 106, 4, 0, 1, 2, 123};
    auto encoded = MessageEncoder::encode("blah", 2);

    CHECK_EQUAL(encoded.size(), 0);
}

TEST(TestMessageEncoder, ShouldReturnEmptyDataWhenThereIsNothingToDecode)  // NOLINT
{
    std::string expectedType = "";
    constexpr size_t expectedBytesToConsume = 0;
    std::vector<uint8_t> noWaterMsg = {};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}


TEST(TestMessageEncoder, ShouldDecodeMsgWhenJustMsgIsInData)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 12;
    std::vector<uint8_t> noWaterMsg = {85, 170, 3, 7, 0, 5, 101, 4, 0, 1, 0, 120};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWhenOtherDataIsInFront)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 16;
    std::vector<uint8_t> noWaterMsg = {10, 20, 30, 40, 85, 170, 3, 7, 0, 5, 101, 4, 0, 1, 0, 120};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWhenOtherDataIsAtEnd)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 12;
    std::vector<uint8_t> noWaterMsg = {85, 170, 3, 7, 0, 5, 101, 4, 0, 1, 0, 120, 11, 22, 33, 44};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWhenSuroundedByOtherData)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 15;
    std::vector<uint8_t> noWaterMsg
        = {10, 20, 30, 85, 170, 3, 7, 0, 5, 101, 4, 0, 1, 0, 120, 11, 22, 33, 44};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWhenMoreMsgInData)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 15;
    std::vector<uint8_t> noWaterMsg = {10,  20, 30,  85, 170, 3, 7, 0,   5, 101, 4, 0, 1,  0,
                                       120, 85, 170, 3,  7,   0, 5, 101, 4, 0,   1, 0, 120};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWhenMoreSeparatedMsgInData)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 15;
    std::vector<uint8_t> noWaterMsg = {10, 20, 30, 85,  170, 3, 7, 0, 5,   101, 4, 0, 1, 0,  120,
                                       11, 22, 85, 170, 3,   7, 0, 5, 101, 4,   0, 1, 0, 120};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(noWaterMsg);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, SouldReturnEmptyMessageWhileThereIsNoPreamble)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 5;

    std::vector<uint8_t> garbateData = {11, 22, 33, 44, 55};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(garbateData);

    CHECK_EQUAL(msg.type, "");
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, SouldReturnEmptyMessageWhileNotFullMsgInBuffer)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 2;

    std::vector<uint8_t> garbateData = {11, 22, 85, 170, 3, 7, 0, 5, 101, 4, 0, 1};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(garbateData);

    CHECK_EQUAL(msg.type, "");
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, SouldReturnEmptyMessageWhileChecksumIsWrong)  // NOLINT
{
    std::string expectedType = "water_lvl";
    constexpr size_t expectedBytesToConsume = 12;
    constexpr uint8_t wrongChecksum = 123;
    std::vector<uint8_t> garbateData = {85, 170, 3, 7, 0, 5, 101, 4, 0, 1, 0, wrongChecksum};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(garbateData);

    CHECK_EQUAL(msg.type, "");
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}

TEST(TestMessageEncoder, ShouldDecodeMsgWithEmptyPayload)  // NOLINT
{
    std::string expectedType = "i_am_alive";
    constexpr size_t expectedBytesToConsume = 7;
    std::vector<uint8_t> iAmAlive = {85, 170, 0, 0, 0, 0, 255};
    auto [msg, readedBytes] = MessageEncoder::decodeFirsMsgInRange(iAmAlive);

    CHECK_EQUAL(msg.type, expectedType);
    CHECK_EQUAL(msg.value, 0);
    CHECK_EQUAL(readedBytes, expectedBytesToConsume);
}