#pragma once

#include <array>
#include <cinttypes>
#include <concepts>
#include <span>
#include <string>
#include <utility>

template <typename T>
concept CMessageEncoder = requires(std::string type, uint8_t value, std::span<uint8_t> buffer) {
    { T::encode(type, value) } -> std::same_as<std::vector<uint8_t>>;
    { T::decodeFirsMsgInRange(buffer) } -> std::same_as<std::pair<typename T::Msg, size_t>>;
};
