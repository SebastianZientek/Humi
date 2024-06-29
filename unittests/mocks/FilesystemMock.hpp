#pragma once

#include <CppUTestExt/MockSupport.h>

#include "FileMock.hpp"

class FilesystemMock
{
public:
    using FileType = FileMock;

    FileType open(const char *path, const char *mode)
    {
        auto *returnVal = mock("FilesystemMock").actualCall("open").returnPointerValue();
        return *static_cast<FileMock *>(returnVal);
    }
};
