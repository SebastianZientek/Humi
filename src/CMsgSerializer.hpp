#pragma once

#include <concepts>

template<typename T>
concept CMsgSerializer = requires(T serializer) {
    { serializer.getData() } -> std::same_as<std::vector<uint8_t> &>;
};
