#include <Arduino.h>

#include "Web.hpp"
#include "WebServer/EventSrcClient.hpp"
#include "WebServer/WebRequest.hpp"
#include "WebServer/WebServer.hpp"
#include "WifiConfigurator.hpp"

#include "Logger.hpp"

#include "WebPage.hpp"

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600); 
    Logger::init();

    connectToWifi();

    Serial1.println(WiFi.localIP());

    static WebPage<WebServer<EventSrcClient>> webPage;
    webPage.start();

    // static WebServer<WebRequest, EventSrcClient> webServer(80);

    // webServer.onGet("/", [](auto request) { request.send(400, "text/html", "<h1>test</h1>"); });
    // webServer.start();

}

void loop()
{
    Logger::debug("Test");
    delay(1000);
}
