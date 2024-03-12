#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "MessagesSerializer.hpp"

// clang-format off
TEST_GROUP(TestMessages)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

};
// clang-format on

TEST(TestMessages, ShouldPrepareMessageIfTypeIsCorrect)  // NOLINT
{
    MessagesSerializer msg("humidification_power", 2);
    std::vector<uint8_t> expectedContent{85, 170, 0, 6, 0, 5, 106, 4, 0, 1, 2, 123};

    CHECK_TRUE(msg.getData() == expectedContent);
}

TEST(TestMessages, ShouldReturnEmptyDataOnWrongMsgType)  // NOLINT
{
    MessagesSerializer msg("blah", 2);
    std::vector<uint8_t> expectedContent{85, 170, 0, 6, 0, 5, 106, 4, 0, 1, 2, 123};

    CHECK_TRUE(msg.getData().size() == 0);
}
