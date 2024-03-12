#pragma once

#include <concepts>

template<typename T>
concept CResources = requires {
    { T::getIndexHtml() } -> std::same_as<const char*>;
    { T::getPicoCss() } -> std::same_as<const char*>;
    { T::getFavicon() } -> std::same_as<const unsigned char*>;
    { T::getFaviconSize() } -> std::same_as<unsigned int>;
};