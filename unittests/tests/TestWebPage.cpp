#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "WebPage.hpp"
#include "mocks/EventSrcClientMock.hpp"
#include "mocks/ResourcesMock.hpp"
#include "mocks/WebRequestMock.hpp"
#include "mocks/WebServerMock.hpp"

// clang-format off
TEST_GROUP(TestWebPage)  // NOLINT
{
    using WebServer = WebServerMock<WebRequestMock, EventSrcClientMock>;

    void setup() override
    {
        webServerMock = std::make_shared<WebServer>();
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }

    void mockOnGetAndOnPostCalls()
    {
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/");
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/favicon.ico");
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/pico.min.css");
        mock("WebServerMock").expectOneCall("onPost").withParameter("url", "/configure");
    }

    std::shared_ptr<WebServer> webServerMock;
};
// clang-format on

TEST(TestWebPage, ShouldGetIndexHtml)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start();

    mock("ResourcesMock").expectOneCall("getIndexHtml");
    mock("WebRequestMock").expectOneCall("send").withParameter("code", 200).ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callGet("/", webRequest);
}

TEST(TestWebPage, ShouldGetPicoCss)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start();

    mock("ResourcesMock").expectOneCall("getPicoCss");
    mock("WebRequestMock").expectOneCall("send").withParameter("code", 200).ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callGet("/pico.min.css", webRequest);
}

TEST(TestWebPage, NotImplementedYet)  // NOLINT
{
    // WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    // mockOnGetAndOnPostCalls();

    // mock("WebServerMock").expectOneCall("start");
    // webPage.start();

    // mock("ResourcesMock").expectOneCall("getPicoCss");
    // mock("WebRequestMock").expectOneCall("send").withParameter("code", 200).ignoreOtherParameters();

    // WebRequestMock webRequest;
    // webServerMock->callPostWithBody("/configure", webRequest, "");
}
