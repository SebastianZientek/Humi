#pragma once

#include <CppUTestExt/MockSupport.h>

#include <cinttypes>
#include <map>
#include <string>

class WebRequestMock
{
public:
    void send(int code, const std::string &contentType, const uint8_t *content, size_t len)
    {
        mock("WebRequestMock")
            .actualCall("send")
            .withParameter("code", code)
            .withParameter("contentType", contentType.c_str())
            .withParameter("content", content)
            .withParameter("len", len);
    }

    void send(int code, const std::string &contentType, const char *content)
    {
        mock("WebRequestMock")
            .actualCall("send")
            .withParameter("code", code)
            .withParameter("contentType", contentType.c_str())
            .withParameter("content", content);
    }

    void send(int code)
    {
        mock("WebRequestMock").actualCall("send").withParameter("code", code);
    }

    void redirect(const std::string &url)
    {
        mock("WebRequestMock").actualCall("redirect").withParameter("url", url.c_str());
    }

    bool authenticate(const std::string &user, const std::string &passwd)
    {
        return mock("WebRequestMock")
            .actualCall("authenticate")
            .withParameter("user", user.c_str())
            .withParameter("passwd", passwd.c_str())
            .returnBoolValue();
    }

    void requestAuthentication()
    {
        mock("WebRequestMock").actualCall("requestAuthentication");
    }

    std::map<std::string, std::string> getParams()
    {
        auto *value
            = mock("WebRequestMock").actualCall("getParams").returnPointerValueOrDefault({});
        if (value != nullptr)
        {
            return *static_cast<std::map<std::string, std::string> *>(value);
        }

        return {};
    }
};
