#pragma once

#include <CppUTestExt/MockSupport.h>

#include <string>

class ExampleMock
{
    std::string readString() override
    {
        return mock("RaiiFileMock").actualCall("readString").returnStringValueOrDefault("");
    };

    void print(const std::string &str) override
    {
        mock("RaiiFileMock").actualCall("print").withStringParameter("str", str.c_str());
    };
};
