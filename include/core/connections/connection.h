#ifndef __CLIENT_CORE_CONNECTIONS_CONNECTION__
#define __CLIENT_CORE_CONNECTIONS_CONNECTION__

#include <deque>
#include <vector>
#include <cstdint>
#include <memory>
#include <asio/streambuf.hpp>
#include <asio/local/stream_protocol.hpp>

namespace spdlog
{
    class logger;
};

namespace asio
{
    class io_context;
}

namespace core::operations
{
    class operation;
}

namespace core::connections
{
    class connection_listener;
    class connection
    {
        friend core::operations::operation;

    public:
        connection(asio::io_context &context, connection_listener &listener);
        virtual ~connection();
        void connect();
        void disconnect();

    private:
        void wait_to_read_content();
        void read_payload();
        void write(const std::vector<uint8_t> &payload);

    private:
        asio::local::stream_protocol::socket socket;
        connection_listener &listener;
        //asio::streambuf stream_buffer;
        std::vector<uint8_t> buffer;
        std::deque<std::vector<uint8_t>> sending_queue;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_CORE_CONNECTIONS_CONNECTION__