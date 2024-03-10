#include "WebRequest.hpp"

WebRequest::WebRequest(AsyncWebServerRequest *webRequest)
    : m_webRequest(webRequest)
{
}

void WebRequest::send(int code, const std::string &contentType, const uint8_t *content, size_t len)
{
    m_webRequest->send_P(code, contentType.c_str(), content, len);
}

void WebRequest::send(int code, const std::string &contentType, const char *content)
{
    m_webRequest->send_P(code, contentType.c_str(), content);
}

void WebRequest::send(int code)
{
    m_webRequest->send(code);
}

void WebRequest::redirect(const std::string &url)
{
    m_webRequest->redirect(url.c_str());
}

bool WebRequest::authenticate(const std::string &user, const std::string &passwd)
{
    return m_webRequest->authenticate(user.c_str(), passwd.c_str());
}

void WebRequest::requestAuthentication()
{
    m_webRequest->requestAuthentication();
}

std::map<std::string, std::string> WebRequest::getParams()
{
    std::map<std::string, std::string> paramsMap;
    auto paramsNumber = m_webRequest->params();

    for (std::size_t idx = 0; idx < paramsNumber; ++idx)
    {
        AsyncWebParameter *param = m_webRequest->getParam(idx);
        const auto *paramName = param->name().c_str();
        const auto *paramValue = param->value().c_str();
        paramsMap[paramName] = paramValue;  // NOLINT
    }

    return paramsMap;
}
