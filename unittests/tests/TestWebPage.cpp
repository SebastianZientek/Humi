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
        mock("WebServerMock").expectOneCall("onPost").withParameter("url", "/set");
    }

    constexpr static auto HTML_OK = 200;
    constexpr static auto HTML_BAD_REQ = 400;

    std::shared_ptr<WebServer> webServerMock;
    
};
// clang-format on

TEST(TestWebPage, ShouldGetIndexHtml)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    mock("ResourcesMock").expectOneCall("getIndexHtml");
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callGet("/", webRequest);
}

TEST(TestWebPage, ShouldGetPicoCss)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    mock("ResourcesMock").expectOneCall("getPicoCss");
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callGet("/pico.min.css", webRequest);
}

TEST(TestWebPage, HandleGarbageDataThatIsNotJson)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *garbage = "blahblahblah";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, garbage);
}

TEST(TestWebPage, HandleCorrectJsonContainingWrongKeys)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *wrongData = R"({"wrong": "data"})";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButStringInsteadOfUInt)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *wrongData = R"({"type":"some_type","value": "str_instead_of_uint"})";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButIntInsteadOfString)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *wrongData = R"({"type": 123,"value": 123})";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButValueIsNegative)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *wrongData = R"({"type": "some_type","value": -123})";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButValueIsTooBig)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value) {});

    const auto *wrongData = R"({"type": "some_type","value": 1000})";
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectMessage)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();

    mock("WebServerMock").expectOneCall("start");
    webPage.start([](const std::string &msgType, uint8_t value)
                  { mock("Lambda").actualCall("callback"); });

    const auto *wrongData = R"({"type": "some_type","value": 4})";

    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();
    mock("Lambda").expectOneCall("callback");

    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}
