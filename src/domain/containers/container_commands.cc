#include <domain/containers/container_commands.h>
#include <domain/containers/create_command.h>
#include <domain/containers/start_command.h>
#include <domain/containers/stop_command.h>
#include <domain/containers/list_command.h>
#include <domain/containers/remove_command.h>
#include <domain/containers/shell_command.h>
#include <domain/containers/log_command.h>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

namespace domain::containers
{
    container_command::container_command(): verbose(false), logger(spdlog::get("jpod"))
    {
    }

    std::string container_command::name()
    {
        return "container";
    }
    std::string container_command::description()
    {
        return "container operations";
    }
    void container_command::setup(lyra::command &builder)
    {
    }
    void container_command::on_run(const lyra::group &g)
    {
    }
    void container_command::sub_command_setup()
    {
         add_subcommands(std::make_shared<create_command>());
         add_subcommands(std::make_shared<start_command>());
         add_subcommands(std::make_shared<stop_command>());
         add_subcommands(std::make_shared<list_command>());
         add_subcommands(std::make_shared<remove_command>());
         add_subcommands(std::make_shared<shell_command>());
         add_subcommands(std::make_shared<log_command>());
    }
    container_command::~container_command()
    {
    }
}