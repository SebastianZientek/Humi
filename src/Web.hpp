#pragma once

#include <ESPAsyncWebServer.h>

std::array<uint8_t, 12> lOff = {85, 170, 0, 6, 0, 5, 102, 4, 0, 1, 0, 117};
std::array<uint8_t, 12> lMax = {85, 170, 0, 6, 0, 5, 102, 4, 0, 1, 3, 120};

class Web {
public:
  Web() : m_asyncWebServer(80) {}

  void start() {
    m_asyncWebServer.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
      auto params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *param = request->getParam(i);
        if (param->name() == "light") {
          if (param->value() == "on") {
            Serial.write(lMax.data(), lMax.size());
          } else {
            Serial.write(lOff.data(), lOff.size());
          }
        }
      }

      request->send_P(400, "text/html", "<a href='/?light=on'>on</a><br><a href='/?light=off'>off</a>");
    });

    m_asyncWebServer.begin();
  }

private:
  AsyncWebServer m_asyncWebServer;
};