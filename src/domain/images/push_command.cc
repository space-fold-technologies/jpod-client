#include <domain/images/push_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    push_command::push_command() : progress(true), image_name(""), tag("latest"), registry(""), token(""), logger(spdlog::get("jpod"))
    {
    }
    std::string push_command::name()
    {
        return "push";
    }
    std::string push_command::description()
    {
        return "push local image to registry";
    }
    void push_command::setup(lyra::command &builder)
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
        builder.add_argument(lyra::opt(image_name, "name")
                                 .name("-n")
                                 .name("--name")
                                 .required()
                                 .help(
                                     "Name of the image to be built."));
        builder.add_argument(lyra::opt(tag, "tag")
                                 .name("-t")
                                 .name("--tag")
                                 .optional()
                                 .help(
                                     "Human readable image tag."));
        builder.add_argument(lyra::opt(registry, "registry")
                                 .name("-r")
                                 .name("--registry")
                                 .optional()
                                 .help(
                                     "Target registry"));
        builder.add_argument(lyra::opt(token, "token")
                                 .name("-a")
                                 .name("--token")
                                 .optional()
                                 .help(
                                     "One time token `useful in CI deployment scenarios`"));
    }
    void push_command::on_run(const lyra::group &g)
    {
        logger->info("push command called");
    }
    push_command::~push_command()
    {
    }
}