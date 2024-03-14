#include <domain/images/remove_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    remove_command::remove_command() : verbose(false), logger(spdlog::get("jpod"))
    {
    }
    std::string remove_command::name()
    {
        return "rm";
    }
    std::string remove_command::description()
    {
        return "remove local image";
    }
    void remove_command::setup(lyra::command &builder)
    {
        builder.add_argument(lyra::opt(verbose)
                                 .name("-v")
                                 .name("--verbose")
                                 .optional()
                                 .help(
                                     "Show additional output as to what we are doing."));
        builder.add_argument(lyra::opt(verbose)
                                 .name("-p")
                                 .name("--progress")
                                 .optional()
                                 .help(
                                     "Show progress."));
    }
    void remove_command::on_run(const lyra::group &g)
    {
        logger->info("remove command called");
    }
    remove_command::~remove_command()
    {
    }
}