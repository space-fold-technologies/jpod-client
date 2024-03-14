#include <domain/images/list_command.h>
#include <core/operations/operation.h>
#include <core/commands/formatter.h>
#include <domain/images/payloads.h>
#include <lyra/lyra.hpp>
#include <fmt/format.h>
#include <range/v3/algorithm/for_each.hpp>
#include <tabulate/table.hpp>

using namespace core::connections;
using namespace tabulate;

namespace domain::images
{
    list_command::list_command() : query(""), operation(nullptr)
    {
    }
    std::string list_command::name()
    {
        return "ls";
    }
    std::string list_command::description()
    {
        return "list images matching pattern";
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
    void list_command::on_operation_started()
    {
        image_term_order order{query};
        auto content = pack_image_term_order(order);
        operation->write_order(operation_target::image, request_operation::list, content);
    }
    void list_command::on_operation_data_received(
        core::connections::operation_target target,
        core::connections::response_operation op,
        const std::vector<uint8_t> &payload)
    {
        operation->shutdown();
        auto summary = unpack_summary(payload);
        Table table;
        table.add_row({"NAME", "REPOSITORY", "TAG", "IMAGE ID", "CREATED", "SIZE"});
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
                               entry.repository,
                               entry.tag,
                               entry.identifier,
                               core::commands::format_time_ago(entry.created_at),
                               core::commands::format_file_size(entry.size)});
            });
        std::cout << table << std::endl;
    }
    void list_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
    }
    void list_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            //logger->error("list command operation failure: {} {}", error.message(), details);
        }
    }
    void list_command::on_operation_success(const std::string &payload)
    {
    }
    list_command::~list_command()
    {
        // we will do some cleaning here if necessary
    }
}