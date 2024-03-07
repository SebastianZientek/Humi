#include "EventSrcClient.hpp"

EventSrcClient::EventSrcClient(AsyncEventSourceClient *client)
    : m_client(client)
{
}
uint32_t EventSrcClient::lastId()
{
    return m_client->lastId();
}

void EventSrcClient::send(const char *message,
                          const char *event,
                          uint32_t identifier,
                          uint32_t reconnect)
{
    m_client->send(message, event, identifier, reconnect);
}
