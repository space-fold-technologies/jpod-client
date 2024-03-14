#include <domain/containers/remove_command.h>
#include <core/operations/operation.h>
#include <domain/containers/payloads.h>
#include <lyra/lyra.hpp>
#include <fmt/color.h>

using namespace core::connections;

namespace domain::containers
{
    remove_command::remove_command() : force(false)
    {
    }

    std::string remove_command::name()
    {
        return "rm";
    }
    std::string remove_command::description()
    {
        return "command to remove a container";
    }
    void remove_command::setup(lyra::command &builder)
    {
        builder.add_argument(lyra::opt(force)
                                 .name("-f")
                                 .name("--force")
                                 .optional()
                                 .help("force flag."));
        builder.add_argument(
            lyra::opt(container_name, "name")
                ["-n"]["--name"]
                    .required()("unique name for container {or identifier}"));
    }
    void remove_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        operation = std::make_unique<core::operations::operation>(*this);
        operation->initialize();
    }

    // callbacks
    void remove_command::on_operation_started()
    {
        if (!container_name.empty())
        {
            container_remove_order order{container_name, force};
            auto content = pack_container_remove_order(order);
            operation->write_order(operation_target::container, request_operation::start, content);
        }
        else
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘ no container identifier specified!\n");
        }
    }
    void remove_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation operation,
        const std::vector<uint8_t> &payload)
    {
    }
    void remove_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘ no container {}!\n", details);
        }
    }
    void remove_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
    }
    void remove_command::on_operation_success(const std::string &payload)
    {
        operation->shutdown();
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", payload);
    }
    remove_command::~remove_command()
    {
    }

}
