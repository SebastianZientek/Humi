#pragma once

#include <concepts>

template<typename T>
concept CUart = requires(T device, const uint8_t *buffer, size_t size) {
    { device.write(buffer, size) } -> std::same_as<size_t>;
    { device.available() } -> std::same_as<int>;
    { device.read() } -> std::convertible_to<int>;
};