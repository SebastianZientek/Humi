#pragma once

#include <concepts>
#include <map>
#include <string>

template<typename T>
concept IWebRequest = requires(T request) {
    { request.send(std::declval<int>(), std::declval<const std::string&>(), std::declval<const uint8_t*>(), std::declval<size_t>()) } -> std::same_as<void>;
    { request.send(std::declval<int>(), std::declval<const std::string&>(), std::declval<const char*>()) } -> std::same_as<void>;
    { request.send(std::declval<int>()) } -> std::same_as<void>;
    { request.redirect(std::declval<const std::string&>()) } -> std::same_as<void>;
    { request.authenticate(std::declval<const std::string&>(), std::declval<const std::string&>()) } -> std::same_as<bool>;
    { request.requestAuthentication() } -> std::same_as<void>;
    { request.getParams() } -> std::same_as<std::map<std::string, std::string>>;
};
