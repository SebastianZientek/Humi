#pragma once

#include <ESPAsyncWebServer.h>

#include <string>

#include "CEventSrcClient.hpp"
#include "CWebRequest.hpp"
#include "CWebServer.hpp"

template <CWebRequest RequestType, CEventSrcClient EventSrcClientType>
class WebServer
{
public:
    using Request = RequestType;
    using EventSrcClient = EventSrcClientType;

    using RequestClbk = std::function<void(Request &)>;
    using RequestClbkWithBody = std::function<void(Request &, std::string)>;

    WebServer(uint16_t port)
        : m_server(port)
    {
    }

    void start()
    {
        m_server.begin();
    }

    void stop()
    {
        if (m_events)
        {
            m_events->close();
        }
        m_server.end();
    }

    void onGet(const std::string &url, const RequestClbk &clbk)
    {
        m_server.on(url.c_str(), HTTP_GET,
                    [clbk](AsyncWebServerRequest *request)
                    {
                        auto req = Request(request);
                        clbk(req);
                    });
    }

    void onPost(const std::string &url, const RequestClbk &clbk)
    {
        m_server.on(url.c_str(), HTTP_POST,
                    [clbk](AsyncWebServerRequest *request)
                    {
                        auto req = Request(request);
                        clbk(req);
                    });
    }

    void onPost(const std::string &url, const RequestClbkWithBody &clbk)
    {
        m_server.on(
            url.c_str(), HTTP_POST, [](AsyncWebServerRequest *request) {},
            [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
               size_t len, bool final) {},
            [clbk](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index,
                   size_t total)
            {
                auto req = Request(request);
                auto body = std::string(data, data + len);
                clbk(req, body);
            });
    }

    void setupEventsSource(const std::string &src,
                           const std::function<void(EventSrcClient &)> &onConnectClbk)
    {
        m_events = std::make_unique<AsyncEventSource>(src.c_str());
        m_events->onConnect(
            [onConnectClbk](AsyncEventSourceClient *client)
            {
                auto eventSrcClient = EventSrcClient(client);
                onConnectClbk(eventSrcClient);
            });

        m_server.addHandler(m_events.get());
    }

    void sendEvent(const char *message,
                   const char *event = nullptr,
                   uint32_t identifier = 0,
                   uint32_t reconnect = 0)
    {
        m_events->send(message, event, identifier, reconnect);
    }

private:
    AsyncWebServer m_server;
    std::unique_ptr<AsyncEventSource> m_events;
};
