#pragma once

#include <concepts>
#include <string>
#include <cinttypes>

template<typename T>
concept CMessageEncoder = requires(std::string type, uint8_t value){
    { T::encode(type, value) } -> std::same_as<std::vector<uint8_t>>;
};
