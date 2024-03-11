#include "App.hpp"

#include <Arduino.h>

#include "Logger.hpp"
#include "WifiConfigurator.hpp"

void App::init()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Logger::init([](const std::string &txt) { Serial1.println(txt.c_str()); });

    WifiConfigurator::connectToWifi();
    Logger::info("Local IP {}", WiFi.localIP().toString().c_str());
    m_webPage.start([](const std::string &msgType, uint8_t value) {});
}

void App::update()
{
    Logger::debug("Alive");
    delay(5000);
}
