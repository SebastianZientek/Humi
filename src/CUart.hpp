#pragma once

#include <concepts>

template<typename T>
concept CUart = requires(T device, const uint8_t *buffer, size_t size) {
    // Check for different variants of read functions
    // { device.read() } -> std::convertible_to<int>;
    // { device.read(reinterpret_cast<char*>(nullptr), size_t{0}) } -> std::convertible_to<size_t>;
    // { device.read(reinterpret_cast<uint8_t*>(nullptr), size_t{0}) } -> std::convertible_to<size_t>;

    // Check for different variants of write functions
    // { device.write('c') } -> std::convertible_to<size_t>;
    // { device.write(reinterpret_cast<const char*>(nullptr), size_t{0}) } -> std::convertible_to<size_t>;
    // { device.write(reinterpret_cast<const uint8_t*>(nullptr), size_t{0}) } -> std::convertible_to<size_t>;


    { device.write(buffer, size) } -> std::convertible_to<size_t>;
    // { device.available() } -> std::convertible_to<int>;
    // { device.print("example") } -> std::convertible_to<size_t>;
};