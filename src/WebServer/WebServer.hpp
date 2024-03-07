#pragma once

#include <ESPAsyncWebServer.h>

#include <string>

#include "IEventSrcClient.hpp"
#include "IWebRequest.hpp"

template <IWebRequest WebRequestT, IEventSrcClient EventSrcClientT>
class WebServer
{
public:
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

    void onGet(const std::string &url, auto clbk)
    {
        // m_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send(400, "text/html", "<h1>test</h1>");});

        m_server.on(url.c_str(), HTTP_GET,
                    [clbk](AsyncWebServerRequest *request) { clbk(WebRequestT(request)); });
    }
    void onPost(const std::string &url, auto clbk)
    {
        m_server.on(url.c_str(), HTTP_POST,
                    [clbk](AsyncWebServerRequest *request) { clbk(WebRequestT(request)); });
    }
    void onPostWithBody(const std::string &url, auto clbk)
    {
        m_server.on(
            url.c_str(), HTTP_POST, [](AsyncWebServerRequest *request) {},
            [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
               size_t len, bool final) {},
            [clbk](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index,
                   size_t total)
            {
                auto req = WebRequestT(request);
                auto body = std::string(data, data + len);
                clbk(req, body);
            });
    }

    void setupEventsSource(const std::string &src, auto onConnectClbk)
    {
        m_events = std::make_unique<AsyncEventSource>(src.c_str());
        m_events->onConnect(
            [onConnectClbk](AsyncEventSourceClient *client)
            {
                auto eventSrcClient = EventSrcClientT(client);
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
