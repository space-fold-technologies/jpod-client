#include <domain/containers/list_command.h>
#include <core/operations/operation.h>
#include <core/commands/formatter.h>
#include <domain/containers/payloads.h>
#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <range/v3/algorithm/for_each.hpp>
#include <tabulate/table.hpp>

using namespace core::connections;
using namespace tabulate;

namespace domain::containers
{
    list_command::list_command() : query("")
    {
    }
    std::string list_command::name()
    {
        return "ls";
    }
    std::string list_command::description()
    {
        return "list containers matching pattern";
    }
    void list_command::setup(lyra::command &builder)
    {
        builder.add_argument(lyra::opt(query, "query")
                                 .name("-q")
                                 .name("--query")
                                 .optional()
                                 .help(
                                     "Query to filter."));
    }
    void list_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        operation = std::make_unique<core::operations::operation>(*this);
        operation->initialize();
    }

    // callbacks
    void list_command::on_operation_started()
    {
        container_list_order order{filter::all, query};
        auto content = pack_container_list_order(order);
        operation->write_order(operation_target::container, request_operation::list, content);
    }
    void list_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation op,
        const std::vector<uint8_t> &payload)
    {
        // we will need to cut off operations at this point
        operation->shutdown();
        auto summary = unpack_container_summary(payload);

        Table table;
        table.add_row({"NAME", "IMAGE", "ID", "STATUS", "PORTS", "CREATED"});
        for (std::size_t i = 0; i < 6; ++i)
        {
            table.format()
                .font_color(Color::yellow)
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold});
        }
        ranges::for_each(
            summary.entries,
            [&table](const auto &entry)
            {
                table.add_row({entry.name,
                               entry.image,
                               entry.identifier,
                               entry.status,
                               core::commands::format_port_mapping(entry.port_map),
                               core::commands::format_time_ago(entry.created_at)});
            });
        std::cout << table << std::endl;
    }
    void list_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            // we will need to cut off operations at this point
            operation->shutdown();
        }
    }
    void list_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
    }
    void list_command::on_operation_success(const std::string &payload)
    {
    }
    list_command::~list_command()
    {
    }
}