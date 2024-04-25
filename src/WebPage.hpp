#pragma once

#include <limits>
#include <memory>
#include <nlohmann/json.hpp>

#include "CResources.hpp"
#include "Logger.hpp"
#include "WebServer/CWebServer.hpp"

template <CWebServer WebServer, CResources Resources>
class WebPage
{
    constexpr static auto HTML_OK = 200;
    constexpr static auto HTML_BAD_REQ = 400;

public:
    using ConfigureClbk = std::function<bool(const std::string &msgType, uint8_t value)>;
    using InitEventClbk = std::function<void()>;

    explicit WebPage(const std::shared_ptr<WebServer> &webServer)
        : m_server(webServer)
    {
    }

    void start(const ConfigureClbk &onConfigureClbk, const InitEventClbk &onInitEventClbk)
    {
        m_onConfigureClbk = onConfigureClbk;
        m_onInitEventClbk = onInitEventClbk;

        m_server->onGet("/",
                        [this](WebServer::Request &request)
                        {
                            Logger::debug("get /");
                            request.send(HTML_OK, "text/html", Resources::getIndexHtml());
                        });

        m_server->onGet("/main.js",
                        [this](WebServer::Request &request)
                        {
                            Logger::debug("get /");
                            request.send(HTML_OK, "application/javascript", Resources::getMainJs());
                        });

        m_server->onGet("/favicon.ico",
                        [this](WebServer::Request &request)
                        {
                            Logger::debug("get /favicon.ico");
                            request.send(HTML_OK, "image/png", Resources::getFavicon(),
                                         Resources::getFaviconSize());
                        });

        m_server->onGet("/pico.min.css",
                        [this](WebServer::Request &request)
                        {
                            Logger::debug("get /pico.min.css");
                            request.send(HTML_OK, "text/css", Resources::getPicoCss());
                        });

        m_server->onPost("/set",
                         [this](WebServer::Request &request, const std::string &body)
                         {
                             Logger::debug("get /set {}", body);
                             onSet(request, body);
                         });

        m_server->setupEventsSource(
            "/events",
            [this](WebServer::EventSrcClient &client)
            {
                Logger::debug("Client connected");
                if (client.lastId() != 0)
                {
                    Logger::debug("Client reconnected, last ID: {}", client.lastId());
                }
                m_onInitEventClbk();
            });

        m_server->start();
    }

    void sendEvent(const char *event, const char *message)
    {
        m_server->sendEvent(message, event, m_eventIdentifier++);
    }

    void stop()
    {
        m_server->stop();
    }

private:
    constexpr static auto port = 80;
    size_t m_eventIdentifier = 1;
    std::shared_ptr<WebServer> m_server;
    ConfigureClbk m_onConfigureClbk;
    InitEventClbk m_onInitEventClbk;

    void onSet(WebServer::Request &request, const std::string &body)
    {
        auto message = nlohmann::json::parse(body, nullptr, false);
        if (message.is_discarded())
        {
            Logger::error("Can't parse json data, {}", body);
            request.send(HTML_BAD_REQ);
            return;
        }

        if (!message.contains("type") || !message.contains("value"))
        {
            Logger::error("Can't parse json data, {}", body);
            request.send(HTML_BAD_REQ);
            return;
        }

        if (!message["type"].is_string() || !message["value"].is_number_unsigned())
        {
            Logger::error("Can't parse json data, {}", body);
            request.send(HTML_BAD_REQ);
            return;
        }

        auto msgType = message["type"];
        auto value = message["value"].get<unsigned>();

        auto maxValue = std::numeric_limits<uint8_t>::max();
        if (value > maxValue)
        {
            Logger::error("Can't parse json data, {}", body);
            request.send(HTML_BAD_REQ);
            return;
        }

        auto sended = m_onConfigureClbk(msgType, value);
        if (!sended)
        {
            request.send(HTML_BAD_REQ);
            return;
        }

        request.send(HTML_OK);
    }
};
