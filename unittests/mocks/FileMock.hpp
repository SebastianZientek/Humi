#pragma once

#include <CppUTestExt/MockSupport.h>

#include <string>

class FileMock
{
public:
    std::string readAsString()
    {
        return mock("FileMock").actualCall("readAsString").returnStringValue();
    }

    void print(const std::string &str)
    {
        mock("FileMock").actualCall("print").withParameter("str", str.c_str());
    }

    void close()
    {
        mock("FileMock").actualCall("close");
    }

    bool operator!()
    {
        return !mock("FileMock").actualCall("isFileOk").returnBoolValue();
    }
};
