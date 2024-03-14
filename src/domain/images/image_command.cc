#include <domain/images/image_command.h>
#include <domain/images/list_command.h>
#include <domain/images/build_command.h>
#include <domain/images/details_command.h>
#include <domain/images/pull_command.h>
#include <domain/images/push_command.h>
#include <domain/images/remove_command.h>
#include <domain/images/import_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::images
{
    image_command::image_command() : verbose(false), logger(spdlog::get("jpod"))
    {
    }
    std::string image_command::name()
    {
        return "image";
    }
    std::string image_command::description()
    {
        return "image operations";
    }
    void image_command::setup(lyra::command &builder)
    {
        // logger->info("image command setup");
    }
    void image_command::on_run(const lyra::group &g)
    {
        // might need tp look into adding extra commands
        logger->info("top arguments parsed");
    }
    void image_command::sub_command_setup()
    {
        add_subcommands(std::make_shared<build_command>());
        add_subcommands(std::make_shared<details_command>());
        add_subcommands(std::make_shared<push_command>());
        add_subcommands(std::make_shared<pull_command>());
        add_subcommands(std::make_shared<list_command>());
        add_subcommands(std::make_shared<remove_command>());
        add_subcommands(std::make_shared<import_command>());
    }
    image_command::~image_command()
    {
        // Go nuclear here :)
    }
}