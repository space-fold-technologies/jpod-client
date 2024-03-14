#include <domain/images/build_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    build_command::build_command() : progress(true), format("table"), image_name(""), tag("latest"), logger(spdlog::get("jpod"))
    {
    }
    std::string build_command::name()
    {
        return "build";
    }
    std::string build_command::description()
    {
        return "build image from description field";
    }
    void build_command::setup(lyra::command &builder)
    {
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
    }
    void build_command::on_run(const lyra::group &g)
    {
        logger->info("build command called");
    }
    build_command::~build_command()
    {
    }
}