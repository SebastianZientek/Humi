#pragma once

#include <memory>

#include "Resources.hpp"
#include "WebPage.hpp"
#include "WebServer/EventSrcClient.hpp"
#include "WebServer/WebRequest.hpp"
#include "WebServer/WebServer.hpp"

class App
{
public:
    void init();
    void update();

private:
    using WebSrv = WebServer<WebRequest, EventSrcClient>;
    using MainWebPage = WebPage<WebSrv, Resources>;

    std::shared_ptr<WebSrv> m_webSrv{std::make_shared<WebSrv>(80)};
    MainWebPage m_webPage{m_webSrv};
};