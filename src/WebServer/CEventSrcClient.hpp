#pragma once

#include <concepts>
#include <cstdint>

template <typename T>
concept CEventSrcClient = requires(
    T client, const char *message, const char *event, uint32_t identifier, uint32_t reconnect) {
    {
        client.lastId()
    } -> std::same_as<uint32_t>;
    {
        client.send(message, event, identifier, reconnect)
    } -> std::same_as<void>;
};
