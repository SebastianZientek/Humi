#include "WifiConfigurator.hpp"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "Arduino.h"

void WifiConfigurator::connectToWifi()
{
    // static WiFiManager wifiManager;
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(false);

    constexpr auto delayBeforeResetUs = 5000;
    if (!wifiManager.autoConnect("HumiConnectToWiFi"))
    {
        delay(delayBeforeResetUs);
        ESP.reset();
    }
}

void WifiConfigurator::startConfigPortal()
{
    WiFi.disconnect();

    WiFiManager wifiManager;
    wifiManager.setDebugOutput(false);
    wifiManager.startConfigPortal("HumiConnectToWiFi");
}
