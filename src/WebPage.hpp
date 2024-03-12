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
    constexpr static auto HTML_UNAUTH = 401;
    constexpr static auto HTML_NOT_FOUND = 404;
    constexpr static auto HTML_INTERNAL_ERR = 500;
    constexpr static auto RECONNECT_TIMEOUT = 10000;

public:
    using ConfigureClbk = std::function<void(const std::string &msgType, uint8_t value)>;

    explicit WebPage(const std::shared_ptr<WebServer> &webServer)
        : m_server(webServer)
    {
    }

    void start(const ConfigureClbk &onConfigureClbk)
    {
        m_onConfigureClbk = onConfigureClbk;

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

        m_server->start();
    }

private:
    constexpr static auto port = 80;
    std::shared_ptr<WebServer> m_server;
    ConfigureClbk m_onConfigureClbk;

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

        m_onConfigureClbk(msgType, value);

        request.send(HTML_OK);
    }
};
