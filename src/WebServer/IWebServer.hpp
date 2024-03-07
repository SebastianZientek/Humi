#include <cstdint>
#include <string>

// Define the concept for WebServer
template<typename CallbackGet, typename CallbackPost, typename CallbackPostWithBody, typename CallbackEventSource>
concept IWebServer = requires(const std::string& url, uint32_t identifier, uint32_t reconnect,
                              const char* message, const char* event, CallbackGet getClbk,
                              CallbackPost postClbk, CallbackPostWithBody postWithBodyClbk,
                              CallbackEventSource eventSourceClbk) {
    { getClbk(url) } -> std::same_as<void>;
    { postClbk(url) } -> std::same_as<void>;
    { postWithBodyClbk(url, std::string()) } -> std::same_as<void>;
    { eventSourceClbk(std::string()) } -> std::same_as<void>;
    { eventSourceClbk(std::string(), std::string()) } -> std::same_as<void>;
    { eventSourceClbk(std::string(), std::string(), identifier, reconnect) } -> std::same_as<void>;
};