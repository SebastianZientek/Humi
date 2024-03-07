#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

// clang-format off
TEST_GROUP(TestExample)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

TEST(TestExample, ShouldFail)  // NOLINT
{
    CHECK_TRUE(false);
}
