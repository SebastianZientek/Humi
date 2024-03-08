#pragma once

#include <ESPAsyncWebServer.h>

#include <map>
#include <string>

#include "IWebRequest.hpp"

class WebRequest : public IWebRequest
{
public:
    explicit WebRequest(AsyncWebServerRequest *WebRequest);
    void send(int code,
              const std::string &contentType,
              const uint8_t *content,
              size_t len) override;
    void send(int code, const std::string &contentType, const char *content) override;
    void send(int code) override;
    void redirect(const std::string &url) override;
    bool authenticate(const std::string &user, const std::string &passwd) override;
    void requestAuthentication() override;
    std::map<std::string, std::string> getParams() override;

private:
    AsyncWebServerRequest *m_WebRequest;
};
