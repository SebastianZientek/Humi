#pragma once

#include <memory>

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
    explicit WebPage(const std::shared_ptr<WebServer> &webServer)
        : m_server(webServer)
    {
    }

    void start()
    {
        m_server->onGet("/",
                        [this](WebServer::Request &request)
                        {
                            Logger::debug("get /");
                            request.send(HTML_OK, "text/html", Resources::getIndexHtml());
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

        m_server->onPost("/configure",
                         [this](WebServer::Request &request, const std::string &body)
                         {
                             Logger::debug("get /configure");
                             request.send(HTML_OK);
                         });

        m_server->start();
    }

private:
    constexpr static auto port = 80;
    std::shared_ptr<WebServer> m_server;
};
