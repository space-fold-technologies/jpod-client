#include <core/connections/connection.h>
#include <core/connections/connection_listener.h>
#include <asio/io_context.hpp>
#include <asio/connect.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace core::connections
{
    connection::connection(asio::io_context &context, connection_listener &listener) : socket(context),
                                                                                       listener(listener),
                                                                                       buffer(10 * 1024),
                                                                                       logger(spdlog::get("jpod"))
    {
    }
    void connection::connect()
    {
        // asio::local::stream_protocol::endpoint endpoint(fmt::format("/home/william/Projects/demonstration/unix.socket.jpod.{}", sole::uuid4().str()));
        auto domain_communication_file = std::string("/home/william/Projects/demonstration/unix.socket.jpod");
        asio::local::stream_protocol::endpoint endpoint(domain_communication_file);

        // you will need to create the endpoint with a unique uid.
        socket.async_connect(endpoint, [this](const std::error_code &err)
                             {
            if(!err)
            {
                this->listener.on_connection_open();
                read_payload();
            } else {
                this->listener.on_connection_error(err);
            } });
    }
    void connection::write(const std::vector<uint8_t> &payload)
    {
        // for now we will just write immediately instead of using a queue
        socket.async_write_some(asio::buffer(payload), [this](const std::error_code &err, std::size_t bytes_transferred)
                                {
            if (!err)
                {
                    if (bytes_transferred > 0)
                    {
                        logger->info("wrote : {}", bytes_transferred);
                    }
                }
                else
                {
                    this->listener.on_connection_error(err);
                } });
    }
    void connection::disconnect()
    {
        if (socket.is_open())
        {
            std::error_code err;
            socket.close(err);
            this->listener.on_connection_close();
        }
    }

    void connection::read_payload()
    {
        socket.async_read_some(
            asio::buffer(buffer),
            [this](const std::error_code &err, std::size_t bytes_transferred)
            {
                if (!err)
                {
                    if (bytes_transferred > 0)
                    {
                        this->listener.on_message_received(buffer);
                    }
                    this->read_payload();
                }
                else
                {
                    if (err != asio::error::eof && socket.is_open())
                    {
                        this->listener.on_connection_error(err);
                    }
                }
            });
    }
    connection::~connection()
    {
    }
}