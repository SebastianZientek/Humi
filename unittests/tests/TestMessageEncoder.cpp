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

    CHECK_TRUE(encoded.size() == 0);
}
