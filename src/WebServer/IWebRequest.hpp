#pragma once

#include <cstdint>
#include <map>
#include <string>

class IWebRequest
{
public:
    IWebRequest() = default;
    IWebRequest(const IWebRequest &) = default;
    IWebRequest(IWebRequest &&) = default;
    virtual ~IWebRequest() = default;

    IWebRequest &operator=(const IWebRequest &) = default;
    IWebRequest &operator=(IWebRequest &&) = default;

    virtual void send(int code, const std::string &contentType, const uint8_t *content, size_t len)
        = 0;
    virtual void send(int code, const std::string &contentType, const char *content) = 0;
    virtual void send(int code) = 0;
    virtual void redirect(const std::string &url) = 0;
    virtual bool authenticate(const std::string &user, const std::string &passwd) = 0;
    virtual void requestAuthentication() = 0;
    virtual std::map<std::string, std::string> getParams() = 0;
};
