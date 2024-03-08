#pragma once

#include <CppUTestExt/MockSupport.h>


class WebServerMock {
public:
    explicit WebServerMock(unsigned)
    {

    }

    void start() {
        mock("WebServerMock").actualCall("start");
    }

    void stop() {
        mock("WebServerMock").actualCall("stop");
    }

    void onGet(const std::string &url, const WebRequestClbk &clbk) {
        mock("WebServerMock").actualCall("onGet").withParameter("url", url.c_str());
    }

    void onPost(const std::string &url, const WebRequestClbk &clbk) {
        mock("WebServerMock").actualCall("onPost").withParameter("url", url.c_str());
    }

    void onPost(const std::string &url, const WebRequestWithBodyClbk &clbk) {
        mock("WebServerMock").actualCall("onPostWithBody").withParameter("url", url.c_str());
    }

    // void setupEventsSource(const std::string &src, auto onConnectClbk) {
    //     mock("WebServerMock").actualCall("setupEventsSource").withParameter("src", src);
    // }

    // void sendEvent(const char *message, const char *event = nullptr, uint32_t identifier = 0, uint32_t reconnect = 0) {
    //     mock("WebServerMock").actualCall("sendEvent").withParameter("message", message);
    // }

private:
    // std::map<std::string, IWebServer::WebRequestClbk> m_onGetCallbacks;
    // std::map<std::string, IWebServer::WebRequestClbk> m_onPostCallbacks;
    // std::map<std::string, IWebServer::WebRequestWithBodyClbk> m_onPostWithBodyCallbacks;
};