#pragma once

#include <concepts>
#include <cstdint>
#include <map>
#include <string>

template <typename T>
concept CWebRequest = requires(T request,
                               int code,
                               const std::string &contentType,
                               const uint8_t *content,
                               size_t len,
                               const char *url,
                               const std::string &user,
                               const std::string &passwd) {
    {
        request.send(code, contentType, content, len)
    } -> std::same_as<void>;
    {
        request.send(code, contentType, url)
    } -> std::same_as<void>;
    {
        request.send(code)
    } -> std::same_as<void>;
    {
        request.redirect(url)
    } -> std::same_as<void>;
    {
        request.authenticate(user, passwd)
    } -> std::same_as<bool>;
    {
        request.requestAuthentication()
    } -> std::same_as<void>;
    {
        request.getParams()
    } -> std::same_as<std::map<std::string, std::string>>;
};
