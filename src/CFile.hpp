#pragma once

#include <concepts>
#include <string>

template<typename T>
concept CFile = requires(T file, const std::string& str) {
    { file.readString() } -> std::convertible_to<std::string>;
    { file.print(str.c_str()) };
    { file.close() };
    { !file } -> std::convertible_to<bool>;
};
