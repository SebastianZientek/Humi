#include "WifiConfigurator.hpp"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "Arduino.h"

void connectToWifi()
{
    static WiFiManager wifiManager;
    if (!wifiManager.autoConnect("YourWiFiAP"))
    {
        delay(3000);
        // Reset and try again
        ESP.reset();
        delay(5000);
    }
}
