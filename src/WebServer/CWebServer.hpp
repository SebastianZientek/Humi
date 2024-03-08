#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <concepts>

#include "IWebRequest.hpp"

using WebRequestClbk = std::function<void(IWebRequest &)>;
using WebRequestWithBodyClbk = std::function<void(IWebRequest &, const std::string &body)>;
// using EventClbk = std::function<void(IEventSrcClient &)>;

template <typename T>
concept CWebServer = requires(T server, std::string url, WebRequestClbk clbk, WebRequestWithBodyClbk clbkWithBody) {
    {
        server.start()
    } -> std::same_as<void>;
    {
        server.stop()
    } -> std::same_as<void>;
    {
        server.onGet(url, clbk)
    } -> std::same_as<void>;
    {
        server.onPost(url, clbk)
    } -> std::same_as<void>;
    {
        server.onPost(url, clbkWithBody)
    } -> std::same_as<void>;
    // {
    //     server.setupEventsSource(url, clbk)
    // } -> std::same_as<void>;
    // {
    //     server.sendEvent("message")
    // } -> std::same_as<void>;
};
