#pragma once

#include "Logger.hpp"
#include "Resources.hpp"
#include "WebServer/IWebRequest.hpp"
#include "WebServer/CWebServer.hpp"

template <CWebServer WebServerT>
class WebPage
{
    constexpr static auto HTML_OK = 200;
    constexpr static auto HTML_BAD_REQ = 400;
    constexpr static auto HTML_UNAUTH = 401;
    constexpr static auto HTML_NOT_FOUND = 404;
    constexpr static auto HTML_INTERNAL_ERR = 500;
    constexpr static auto RECONNECT_TIMEOUT = 10000;

public:
    WebPage()
        : m_server(80)
    {
    }

    void start()
    {
        m_server.onGet("/",
                       [this](IWebRequest &request)
                       {
                           Logger::debug("get /");
                           request.send(HTML_OK, "text/html", Resources::getIndexHtml());
                       });

        m_server.onGet("/favicon.ico",
                       [this](IWebRequest &request)
                       {
                           Logger::debug("get /favicon.ico");
                           request.send(HTML_OK, "image/png", Resources::getFavicon(),
                                        Resources::getFaviconSize());
                       });

        m_server.onGet("/pico.min.css",
                       [this](IWebRequest &request)
                       {
                           Logger::debug("get /pico.min.css");
                           request.send(HTML_OK, "text/css", Resources::getPicoCss());
                       });

        m_server.onPost("/configure",
                        [this](IWebRequest &request, const std::string &body)
                        {
                            Logger::debug("get /configure");
                            request.send(HTML_OK);
                        });

        m_server.start();
    }

private:
    WebServerT m_server;
};
