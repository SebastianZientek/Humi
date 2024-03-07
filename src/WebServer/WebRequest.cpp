#include "WebRequest.hpp"

WebRequest::WebRequest(AsyncWebServerRequest *WebRequest)
    : m_WebRequest(WebRequest)
{
}

void WebRequest::send(int code, const std::string &contentType, const uint8_t *content, size_t len)
{
    m_WebRequest->send_P(code, contentType.c_str(), content, len);
}

void WebRequest::send(int code, const std::string &contentType, const char *content)
{
    m_WebRequest->send_P(code, contentType.c_str(), content);
}

void WebRequest::send(int code)
{
    m_WebRequest->send(code);
}

void WebRequest::redirect(const std::string &url)
{
    m_WebRequest->redirect(url.c_str());
}

bool WebRequest::authenticate(const std::string &user, const std::string &passwd)
{
    return m_WebRequest->authenticate(user.c_str(), passwd.c_str());
}

void WebRequest::requestAuthentication()
{
    m_WebRequest->requestAuthentication();
}

std::map<std::string, std::string> WebRequest::getParams()
{
    std::map<std::string, std::string> paramsMap;
    auto paramsNumber = m_WebRequest->params();

    for (std::size_t idx = 0; idx < paramsNumber; ++idx)
    {
        AsyncWebParameter *param = m_WebRequest->getParam(idx);
        const auto *paramName = param->name().c_str();
        const auto *paramValue = param->value().c_str();
        paramsMap[paramName] = paramValue;  // NOLINT
    }

    return paramsMap;
}
