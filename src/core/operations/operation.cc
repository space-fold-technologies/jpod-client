#include <core/operations/operation.h>
#include <core/operations/operation_listener.h>
#include <core/connections/connection.h>
#include <core/connections/frame.h>
#include <core/commands/internal_contracts.h>
#include <spdlog/spdlog.h>

namespace core::operations
{
    operation::operation(operation_listener &listener) : _context(),
                                                         signals(_context, SIGINT, SIGTERM),
                                                         connection(std::make_unique<core::connections::connection>(_context, *this)),
                                                         running(false),
                                                         operation_thread(&operation::run, this),
                                                         listener(listener),
                                                         logger(spdlog::get("jpod"))
    {
    }

    void operation::initialize()
    {
        signals.async_wait(
            [this](const std::error_code &error, int signal_code)
            {
                if (signal_code == SIGTERM)
                {
                    running = false;
                }
            });
        operation_thread.join();
    }
    void operation::write_order(
        core::connections::operation_target target,
        core::connections::request_operation operation,
        const std::vector<uint8_t> &payload)
    {

        auto frame = core::connections::encode_frame(target, operation, payload);
        connection->write(frame);
    }

    bool operation::is_path_valid(const std::string &path)
    {
        // filesystem magic
        return false;
    }
    void operation::run()
    {

        if (!running)
        {
            connection->connect();
            running = true;
            while (running)
            {
                _context.run();
            }
        }
    }

    void operation::shutdown()
    {
        running = false;
        connection->disconnect();
    }

    void operation::on_connection_open()
    {
        listener.on_operation_started();
    }
    void operation::on_connection_close()
    {
        listener.on_operation_complete({}, "");
        running = false;
        _context.stop();
    }
    void operation::on_message_received(const std::vector<uint8_t> &payload)
    {
        auto frame = core::connections::decode_frame(payload);
        switch (frame.operation)
        {
        case core::connections::response_operation::frame:
        {
            listener.on_operation_data_received(frame.target, frame.operation, frame.payload);
            break;
        }
        case core::connections::response_operation::success:
        {
            auto pkg = core::commands::unpack_success_payload(frame.payload);
            listener.on_operation_success(pkg.message);
            break;
        }
        case core::connections::response_operation::progress:
        {
            auto pkg = core::commands::unpack_progress_payload(frame.payload);
            listener.on_progress_update(pkg.operation, pkg.content);
            break;
        }
        case core::connections::response_operation::failure:
        {
            auto error = core::commands::unpack_error_payload(frame.payload);
            listener.on_operation_complete(std::make_error_code(std::errc::state_not_recoverable), error.message);
            break;
        }
        case core::connections::response_operation::close_order:
            break;
        default:
            break;
        }
    }
    void operation::on_connection_error(const std::error_code &error)
    {
        listener.on_operation_complete(error, "network connection failed");
    }
    asio::io_context &operation::context()
    {
        return _context;
    }
    operation::~operation()
    {
        if (connection)
        {
            connection->disconnect();
        }
    }
}