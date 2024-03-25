#include <domain/containers/shell_command.h>
#include <domain/containers/shell/terminal.h>
#include <core/operations/operation.h>
#include <domain/containers/payloads.h>
#include <lyra/lyra.hpp>
#include <fmt/color.h>

using namespace core::connections;

namespace domain::containers
{
    shell_command::shell_command()
    {
    }
    std::string shell_command::name()
    {
        return "shell";
    }
    std::string shell_command::description()
    {
        return "forward an existing containers shell";
    }
    void shell_command::setup(lyra::command &builder)
    {
        builder.add_argument(
            lyra::opt(container_name, "name")
                ["-n"]["--name"]
                    .required()("unique name for container {or identifier}"));
    }
    void shell_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        operation = std::make_unique<core::operations::operation>(*this);
        terminal = std::make_unique<shells::terminal>(operation->context(), *this);
        operation->initialize();
        terminal->initiate();
    }

    // callbacks
    void shell_command::on_operation_started()
    {
        if (!container_name.empty())
        {
            std::vector<uint8_t> data(container_name.begin(), container_name.end());
            container_shell_order order{shell_order_type::start_session, data};
            auto content = pack_container_shell_order(order);
            operation->write_order(operation_target::container, request_operation::shell, content);
        }
        else
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘ no container identifier specified!\n");
        }
    }
    void shell_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation operation,
        const std::vector<uint8_t> &payload)
    {
        if (terminal)
        {
            terminal->write(payload);
        }
    }
    void shell_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "container error:✘ {}!\n", details);
        }
    }
    void shell_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
        // No progress to display
    }
    void shell_command::on_operation_success(const std::string &payload)
    {
        // terminal started in the daemon

        if (payload.find("initialized") != std::string::npos && terminal)
        {
            //fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", payload);
            auto details = terminal->details();
            std::string payload = fmt::format("{}:{}", details.columns, details.rows);
            std::vector<uint8_t> data(payload.begin(), payload.end());
            container_shell_order order{shell_order_type::terminal_size, data};
            auto content = pack_container_shell_order(order);
            operation->write_order(operation_target::container, request_operation::shell, content);
            terminal->start_io();
        }
    }
    void shell_command::on_input_received(const std::vector<uint8_t> &content)
    {
        container_shell_order order{shell_order_type::terminal_feed, content};
        auto payload = pack_container_shell_order(order);
        operation->write_order(operation_target::container, request_operation::shell, payload);
    }
    void shell_command::on_terminal_initialized()
    {
        // terminal initialized
    }
    void shell_command::on_terminal_resized(uint32_t rows, uint32_t columns)
    {
        std::string payload = fmt::format("{}:{}", columns, rows);
        std::vector<uint8_t> data(container_name.begin(), container_name.end());
        container_shell_order order{shell_order_type::start_session, data};
        auto content = pack_container_shell_order(order);
        operation->write_order(operation_target::container, request_operation::shell, content);
    }
    void shell_command::on_terminal_error(const std::error_code &err)
    {
        operation->shutdown();
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "terminal error:✘ {}!\n", err.message());
    }
    shell_command::~shell_command()
    {
    }
}