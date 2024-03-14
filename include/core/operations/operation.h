#ifndef __CLIENT_CORE_OPERATIONS_OPERATION__
#define __CLIENT_CORE_OPERATIONS_OPERATION__

#include <core/connections/connection_listener.h>
#include <core/connections/frame.h>
#include <cstdint>
#include <memory>
#include <thread>
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>

namespace spdlog
{
    class logger;
};

namespace core::connections
{
    class connection;
};

namespace core::operations
{
    class operation_listener;
    class operation : public core::connections::connection_listener
    {

    public:
        operation(operation_listener &listener);
        virtual ~operation();
        void initialize();
        void write_order(
            core::connections::operation_target target,
            core::connections::request_operation operation,
            const std::vector<uint8_t> &payload);
        bool is_path_valid(const std::string &path);
        void shutdown();
        // callbacks
        void on_connection_open() override;
        void on_connection_close() override;
        void on_message_received(const std::vector<uint8_t> &payload) override;
        void on_connection_error(const std::error_code &error) override;

    private:
        void run();

    private:
        asio::io_context context;
        asio::signal_set signals;
        std::unique_ptr<core::connections::connection> connection;
        bool running;
        std::thread operation_thread;
        operation_listener &listener;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_CORE_OPERATIONS_OPERATION__