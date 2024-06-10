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
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/main.js");
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/favicon.ico");
        mock("WebServerMock").expectOneCall("onGet").withParameter("url", "/pico.min.css");
        mock("WebServerMock").expectOneCall("onPost").withParameter("url", "/set");
        mock("WebServerMock").expectOneCall("onPost").withParameter("url", "/mqtt_config");
        mock("WebServerMock").expectOneCall("onPost").withParameter("url", "/ota_config");
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
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("ResourcesMock").expectOneCall("getIndexHtml");
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    webServerMock->callGet("/", webRequest);
}

TEST(TestWebPage, ShouldGetMainJs)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("ResourcesMock").expectOneCall("getMainJs");
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    webServerMock->callGet("/main.js", webRequest);
}

TEST(TestWebPage, ShouldGetPicoCss)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("ResourcesMock").expectOneCall("getPicoCss");
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    webServerMock->callGet("/pico.min.css", webRequest);
}

TEST(TestWebPage, HandleGarbageDataThatIsNotJson)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *garbage = "blahblahblah";
    webServerMock->callPostWithBody("/set", webRequest, garbage);
}

TEST(TestWebPage, HandleCorrectJsonContainingWrongKeys)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *wrongData = R"({"wrong": "data"})";
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButStringInsteadOfUInt)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *wrongData = R"({"type":"some_type","value": "str_instead_of_uint"})";
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButIntInsteadOfString)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    const auto *wrongData = R"({"type": 123,"value": 123})";
    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButValueIsNegative)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    const auto *wrongData = R"({"type": "some_type","value": -123})";
    WebRequestMock webRequest;
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonContainingCorrectKeysButValueIsTooBig)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();

    webPage.start(
        [](const std::string &msgType, uint8_t value) { return true; }, [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *wrongData = R"({"type": "some_type","value": 1000})";
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectJsonWithIncorrectMessageType)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_BAD_REQ)
        .ignoreOtherParameters();
    mock("Lambda").expectOneCall("callback");

    webPage.start(
        [](const std::string &msgType, uint8_t value)
        {
            mock("Lambda").actualCall("callback");
            return false;
        },
        [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *wrongData = R"({"type": "some_type","value": 4})";
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}

TEST(TestWebPage, HandleCorrectMessage)  // NOLINT
{
    WebPage<WebServer, ResourcesMock> webPage(webServerMock);

    mockOnGetAndOnPostCalls();
    mock("WebServerMock").expectOneCall("start");
    mock("WebServerMock").expectOneCall("setupEventsSource").ignoreOtherParameters();
    mock("WebRequestMock")
        .expectOneCall("send")
        .withParameter("code", HTML_OK)
        .ignoreOtherParameters();
    mock("Lambda").expectOneCall("callback");

    webPage.start(
        [](const std::string &msgType, uint8_t value)
        {
            mock("Lambda").actualCall("callback");
            return true;
        },
        [] {},
        [](bool enabled, const std::string &, const std::string &, const std::string &, int) {}, [](bool){});

    WebRequestMock webRequest;
    const auto *wrongData = R"({"type": "some_type","value": 4})";
    webServerMock->callPostWithBody("/set", webRequest, wrongData);
}
