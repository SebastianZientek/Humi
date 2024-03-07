#pragma once

#include <ESPAsyncWebServer.h>

#include <cstdint>

class EventSrcClient
{
public:
    explicit EventSrcClient(AsyncEventSourceClient *client);
    uint32_t lastId();
    void send(const char *message,
              const char *event = nullptr,
              uint32_t identifier = 0,
              uint32_t reconnect = 0);

private:
    AsyncEventSourceClient *m_client;
};
