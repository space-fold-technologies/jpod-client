#include <domain/images/import_command.h>
#include <core/operations/operation.h>
#include <domain/images/payloads.h>
#include <filesystem>
#include <lyra/lyra.hpp>
#include <fmt/color.h>

namespace fs = std::filesystem;
using namespace core::connections;
namespace domain::images
{
    import_command::import_command() : file_path("."),
                                       format("afs"),
                                       operation(nullptr)
    {
    }

    std::string import_command::name()
    {
        return "import";
    }
    std::string import_command::description()
    {
        return "import archived image to be used to create containers";
    }
    void import_command::setup(lyra::command &builder)
    {
        builder.add_argument(lyra::opt(file_path, "archive")
                                 .name("--archive-path")
                                 .required()
                                 .help(
                                     "Image Archive to import."));
        builder.add_argument(lyra::opt(format, "format")
                                 .name("--format")
                                 .required()
                                 .help(
                                     "Format of the Image to be imported [oci, afs]."));
    }
    void import_command::on_run(const lyra::group &g)
    {
        operation = std::make_unique<core::operations::operation>(*this);
        operation->initialize();
    }
    void import_command::on_operation_started()
    {
        fs::path archive_path(file_path);
        std::error_code error;
        if (fs::exists(archive_path, error); error)
        {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "import error: {}!\n", error.message());
        }
        else
        {
            auto content = pack_import_order(import_order{fs::absolute(archive_path).generic_string()});
            operation->write_order(operation_target::image, request_operation::import, content);
        }
    }
    void import_command::on_operation_data_received(cn::operation_target target,
                                                    cn::response_operation operation,
                                                    const std::vector<uint8_t> &payload)
    {
    }
    void import_command::on_operation_complete(const std::error_code &error, const std::string &details)
    {
        if (error)
        {
            operation->shutdown();
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "import error: {}!\n", details);
        }
    }
    void import_command::on_operation_success(const std::string &payload)
    {
        // close operations from here
        fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "\n✔ {} \n", payload);
        operation->shutdown();
    }
    void import_command::on_progress_update(const std::string &operation, const std::vector<uint8_t> &content)
    {
    }
    import_command::~import_command()
    {
    }
}