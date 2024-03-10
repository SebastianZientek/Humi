#pragma once

#include <CppUTestExt/MockSupport.h>

#include <cstdint>

class EventSrcClientMock
{
public:
    uint32_t lastId()
    {
        return mock("EventSrcClientMock").actualCall("lastId").returnUnsignedIntValue();
    }

    void send(const char *message,
              const char *event = nullptr,
              uint32_t identifier = 0,
              uint32_t reconnect = 0)
    {
        mock("EventSrcClientMock")
            .actualCall("send")
            .withParameter("message", message)
            .withParameter("event", event)
            .withParameter("identifier", identifier)
            .withParameter("reconnect", reconnect);
    }
};
