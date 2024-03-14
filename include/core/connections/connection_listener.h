#ifndef __CLIENT_CORE_CONNECTIONS_CONNECTION_LISTENER__
#define __CLIENT_CORE_CONNECTIONS_CONNECTION_LISTENER__

#include <vector>
#include <system_error>

namespace core::connections
{
    class connection_listener
    {
    public:
        virtual void on_connection_open() = 0;
        virtual void on_connection_close() = 0;
        virtual void on_message_received(const std::vector<uint8_t> &payload) = 0;
        virtual void on_connection_error(const std::error_code &error) = 0;
    };
}

#endif // __CLIENT_CORE_CONNECTIONS_CONNECTION_LISTENER__