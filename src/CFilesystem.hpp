#pragma once

#include <concepts>
#include "CMessageEncoder.hpp"
#include "CFile.hpp"


template<typename T>
concept CFilesystem = requires(T filesystem, const std::string& path, const std::string& mode) {
    { filesystem.open(path.c_str(), mode.c_str()) } -> std::same_as<typename T::FileType>;
};
