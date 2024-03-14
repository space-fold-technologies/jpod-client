#include <domain/containers/create_command.h>
#include <domain/containers/payloads.h>
#include <domain/images/payloads.h>
#include <core/operations/operation.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/split.hpp>
#include <fmt/color.h>
#include <lyra/lyra.hpp>

using namespace ranges;
using namespace core::connections;
using namespace domain::images;

namespace domain::containers
{
    create_command::create_command() : env_vars(),
                                       port_maps(),
                                       network_properties("nat"),
                                       operation(nullptr)
    {
    }
    std::string create_command::name()
    {
        return "create";
    }
    std::string create_command::description()
    {
        return "create a new container from a reference image";
    }
    void create_command::setup(lyra::command &builder)
    {
        builder.add_argument(
            lyra::opt(container_name, "name")
                ["-n"]["--name"]
                    .required()("unique name for container"));
        builder.add_argument(
            lyra::opt(image, "image")
                ["-i"]["--image"]
                    .required()("target image to base container on"));
        builder.add_argument(
            lyra::opt(env_vars, "env-vars")
                ["-e"]["--env"]
                    .cardinality(0, 0)("add environment variables"));

        builder.add_argument(
            lyra::opt(port_maps, "port-maps")
                ["-p"]["--port-map"]
                    .cardinality(0, 0)("add port re-directs mapping"));
        builder.add_argument(
            lyra::opt(network_properties, "network")
                ["--network"]
                    .optional()("network properties"));
    }
    void create_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        operation = std::make_unique<core::operations::operation>(*this);
        operation->initialize();
    }

    // callbacks
    void create_command::on_operation_started()
    {
        container_creation_order order;
        order.tagged_image = image;
        order.name = container_name;
        order.network_properties = network_properties;
        auto env_pair = [](const std::string &entry) -> std::pair<std::string, std::string>
        {
            auto parts = views::split(entry, '=') | to<std::vector<std::string>>();
            return std::make_pair(parts.at(0), parts.at(1));
        };
        auto port_map_pair = [](const std::string &entry) -> std::pair<std::string, std::string>
        {
            auto parts = views::split(entry, ':') | to<std::vector<std::string>>();
            return std::make_pair(parts.at(0), parts.at(1));
        };
        if (env_vars.size() > 0)
        {
            order.env_vars = env_vars | views::transform(env_pair) | to<std::map<std::string, std::string>>();
        }
        if (port_maps.size() > 0)
        {
            order.port_map = port_maps | views::transform(port_map_pair) | to<std::map<std::string, std::string>>();
        }
        auto content = pack_container_creation_order(order);
        operation->write_order(operation_target::container, request_operation::build, content);
    }
    void create_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation operation,
        const std::vector<uint8_t> &payload)
    {
    }
    void create_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", details);
        }
    }
    void create_command::on_operation_success(const std::string &payload)
    {
        if (!payload.empty() && payload.size() == 36)
        {
            // this is <<UUID>> which we can use to start the container
            // we just need to send this over using a command
            container_term_order order{payload};
            auto content = pack_container_term_order(order);
            operation->write_order(operation_target::container, request_operation::start, content);
        }
        else
        {
            fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "\n✔{}", payload);
            operation->shutdown();
        }
    }
    void create_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
        auto progress = unpack_progress_frame(content);
        fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic, "\r {}", progress.feed);
    }
    create_command::~create_command()
    {
    }
}