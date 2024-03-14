#include <core/commands/command_registry.h>
#include <core/commands/base_command.h>
#include <range/v3/algorithm/for_each.hpp>
#include <lyra/lyra.hpp>

namespace core::commands
{
    command_registry::command_registry()
    {
    }
    void command_registry::add(std::shared_ptr<base_command> entry)
    {
        this->command_map.emplace(entry->name(), entry);
    }
    void command_registry::register_commands(lyra::cli& cli)
    {
        // register all sub commands with the application
        ranges::for_each(
            command_map,
            [&cli](const auto &entry) {
                    entry.second->initialize(cli);
            });
    }
}