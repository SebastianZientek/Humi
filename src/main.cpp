#include <Arduino.h>

#include "Logger.hpp"
#include "Resources.hpp"
#include "Web.hpp"
#include "WebPage.hpp"
#include "WebServer/EventSrcClient.hpp"
#include "WebServer/WebRequest.hpp"
#include "WebServer/WebServer.hpp"
#include "WifiConfigurator.hpp"

using WebSrv = WebServer<WebRequest, EventSrcClient>;

namespace
{
std::shared_ptr<WebSrv> webSrv{std::make_shared<WebSrv>(80)};
WebPage<WebSrv, Resources> webPage(webSrv);
}  // namespace

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Logger::init();

    connectToWifi();
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());

    webPage.start();
}

void loop()
{
    Logger::debug("Alive");
    delay(5000);
}
