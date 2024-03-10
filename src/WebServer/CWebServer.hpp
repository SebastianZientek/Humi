#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <string>

template <typename Srv>
concept CWebServer = requires(
    Srv srv, std::string url,
    std::function<void(typename Srv::Request)> reqClbk,
    std::function<void(typename Srv::Request, const std::string &body)>
        reqClbkWithBody,
    std::function<void(typename Srv::EventSrcClient &)> onConnectClbk) {
  { srv.start() } -> std::same_as<void>;
  { srv.stop() } -> std::same_as<void>;
  { srv.onGet(url, reqClbk) } -> std::same_as<void>;
  { srv.onPost(url, reqClbk) } -> std::same_as<void>;
  { srv.onPost(url, reqClbkWithBody) } -> std::same_as<void>;
  {
    srv.setupEventsSource(std::declval<const std::string &>(), onConnectClbk)
  } -> std::same_as<void>;
  {
    srv.sendEvent(std::declval<const char *>(), std::declval<const char *>(),
                  std::declval<uint32_t>(), std::declval<uint32_t>())
  } -> std::same_as<void>;
};
