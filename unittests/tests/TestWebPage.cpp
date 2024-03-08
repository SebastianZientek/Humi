#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "WebPage.hpp"
#include "mocks/WebServerMock.hpp"

// clang-format off
TEST_GROUP(TestWebPage)  // NOLINT
{
    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};
// clang-format on

TEST(TestWebPage, ShouldFail)  // NOLINT
{
    static WebPage<WebServerMock> webPage;

    CHECK_TRUE(false);
}
