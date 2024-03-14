#include <domain/images/details_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    details_command::details_command() : verbose(false), progress(false), format("table"), identifier(""), logger(spdlog::get("jpod"))
    {
    }

    std::string details_command::name()
    {
        return "details";
    }
    std::string details_command::description()
    {
        return "details of the image";
    }
    void details_command::setup(lyra::command &builder)
    {
        builder.add_argument(lyra::opt(verbose)
                                 .name("-v")
                                 .name("--verbose")
                                 .optional()
                                 .help(
                                     "Show additional output as to what we are doing."));
        builder.add_argument(lyra::opt(progress, "progress")
                                 .name("-p")
                                 .name("--progress")
                                 .optional()
                                 .help(
                                     "Show progress."));
        builder.add_argument(lyra::opt(format, "format")
                                 .name("-f")
                                 .name("--format")
                                 .choices("json", "table")
                                 .optional()
                                 .help(
                                     "Results display format."));
        builder.add_argument(lyra::arg(identifier, "identifier")
                                 .required()
                                 .help(
                                     "Unique Image ID."));
    }
    void details_command::on_run(const lyra::group &g)
    {
        logger->info("details command called");
    }
    details_command::~details_command()
    {
    }
}