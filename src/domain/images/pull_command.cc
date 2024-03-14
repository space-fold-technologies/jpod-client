#include <domain/images/pull_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    pull_command::pull_command() : progress(true), tagged_name(""), registry(""), token(""), logger(spdlog::get("jpod"))
    {
    }

    std::string pull_command::name()
    {
        return "pull";
    }
    std::string pull_command::description()
    {
        return "pull image from remote registry";
    }
    void pull_command::setup(lyra::command &builder)
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
        builder.add_argument(lyra::opt(registry, "registry")
                                 .name("-r")
                                 .name("--registry")
                                 .optional()
                                 .help(
                                     "Target registry"));
        builder.add_argument(lyra::arg(tagged_name, "name")
                                 .required()
                                 .help(
                                     "Name of the tagged image to be pulled."));
        builder.add_argument(lyra::opt(token, "token")
                                 .name("-a")
                                 .name("--token")
                                 .optional()
                                 .help(
                                     "One time token `useful in CI deployment scenarios`"));
    }
    void pull_command::on_run(const lyra::group &g)
    {
        logger->info("pull command called");
    }
    pull_command::~pull_command()
    {
    }
}