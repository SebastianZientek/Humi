#pragma once

#include <LittleFS.h>

#include <string>

#include "FileAdp.hpp"

class FilesystemAdp
{
public:
    FileAdp open(const std::string &path, const std::string &mode)
    {
        auto file = LittleFS.open(path.c_str(), mode.c_str());
        return FileAdp(file);
    }
};
