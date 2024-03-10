#pragma once

#include <CppUTestExt/MockSupport.h>

#include <functional>
#include <map>

#include "WebServer/CEventSrcClient.hpp"
#include "WebServer/CWebRequest.hpp"

template <CWebRequest RequestType, CEventSrcClient EventSrcClientType>
class WebServerMock
{
public:
    using Request = RequestType;
    using EventSrcClient = EventSrcClientType;

    using RequestClbk = std::function<void(Request &)>;
    using RequestClbkWithBody = std::function<void(Request &, std::string)>;

    void start()
    {
        mock("WebServerMock").actualCall("start");
    }

    void stop()
    {
        mock("WebServerMock").actualCall("stop");
    }

    void onGet(const std::string &url, const RequestClbk &clbk)
    {
        mock("WebServerMock").actualCall("onGet").withParameter("url", url.c_str());
        m_onGetCallbacks[url] = clbk;
    }

    void onPost(const std::string &url, const RequestClbk &clbk)
    {
        mock("WebServerMock").actualCall("onPost").withParameter("url", url.c_str());
        m_onPostCallbacks[url] = clbk;
    }

    void onPost(const std::string &url, const RequestClbkWithBody &clbk)
    {
        mock("WebServerMock").actualCall("onPost").withParameter("url", url.c_str());
        m_onPostWithBodyCallbacks[url] = clbk;
    }

    void setupEventsSource(const std::string &src,
                           const std::function<void(EventSrcClient &)> &onConnectClbk)
    {
        mock("WebServerMock").actualCall("setupEventsSource").withParameter("src", src.c_str());
    }

    void sendEvent(const char *message,
                   const char *event = nullptr,
                   uint32_t identifier = 0,
                   uint32_t reconnect = 0)
    {
        mock("WebServerMock").actualCall("sendEvent").withParameter("message", message);
    }

    // Testability functions

    void callGet(const std::string &url, Request &req)
    {
        m_onGetCallbacks[url](req);
    }

    void callPost(const std::string &url, Request &req)
    {
        m_onPostCallbacks[url](req);
    }

    void callPostWithBody(const std::string &url, Request &req, const std::string &body)
    {
        m_onPostWithBodyCallbacks[url](req, body);
    }

private:
    std::map<std::string, RequestClbk> m_onGetCallbacks;
    std::map<std::string, RequestClbk> m_onPostCallbacks;
    std::map<std::string, RequestClbkWithBody> m_onPostWithBodyCallbacks;
};
