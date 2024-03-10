#pragma once

#include <ESPAsyncWebServer.h>

#include <map>
#include <string>

class WebRequest {
public:
  explicit WebRequest(AsyncWebServerRequest *webRequest);
  void send(int code, const std::string &contentType, const uint8_t *content,
            size_t len);
  void send(int code, const std::string &contentType, const char *content);
  void send(int code);
  void redirect(const std::string &url);
  bool authenticate(const std::string &user, const std::string &passwd);
  void requestAuthentication();
  std::map<std::string, std::string> getParams();

private:
  AsyncWebServerRequest *m_webRequest;
};
