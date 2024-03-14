#include <domain/containers/start_command.h>
#include <core/operations/operation.h>
#include <domain/containers/payloads.h>
#include <lyra/lyra.hpp>
#include <fmt/color.h>

using namespace core::connections;

namespace domain::containers
{
    start_command::start_command()
    {
    }
    std::string start_command::name()
    {
        return "start";
    }
    std::string start_command::description()
    {
        return "start an existing container";
    }
    void start_command::setup(lyra::command &builder)
    {
        builder.add_argument(
            lyra::opt(container_name, "name")
                ["-n"]["--name"]
                    .required()("unique name for container {or identifier}"));
    }
    void start_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        operation = std::make_unique<core::operations::operation>(*this);
        operation->initialize();
    }

    // callbacks
    void start_command::on_operation_started()
    {
        if (!container_name.empty())
        {
            container_term_order order{container_name};
            auto content = pack_container_term_order(order);
            operation->write_order(operation_target::container, request_operation::start, content);
        }
        else
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘ no container identifier specified!\n");
        }
    }
    void start_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation operation,
        const std::vector<uint8_t> &payload)
    {
    }
    void start_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "container error:✘ {}!\n", details);
        }
    }
    void start_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
        // No progress to display
    }
    void start_command::on_operation_success(const std::string &payload)
    {
        operation->shutdown();
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", payload);
    }
    start_command::~start_command()
    {
    }
}