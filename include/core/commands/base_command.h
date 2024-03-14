#ifndef __CLIENT_CORE_BASE_COMMAND__
#define __CLIENT_CORE_BASE_COMMAND__

#include <lyra/lyra.hpp>
#include <memory>
#include <map>
#include <core/commands/sub_command.h>
#include <range/v3/algorithm/for_each.hpp>

namespace core::commands
{
    class command_registry;
    class base_command
    {
        friend command_registry;
        friend sub_command;

    private:
        void initialize(lyra::cli &cli)
        {
            lyra::command cmd(
                this->name(),
                [this](const lyra::group &g)
                {
                    if (this->show_help)
                    {
                        // will think of something to print out here
                    }
                    else
                    {
                        if (sub_commands.empty())
                        {
                            this->on_run(g);
                        }
                    }
                });
            cmd.help(description()).add_argument(lyra::help(show_help));
            sub_command_setup();
            ranges::for_each(sub_commands, [this, &cmd](auto const &entry)
                             { entry.second->initialize(cmd); });
            setup(cmd);
            // Add all sub commands found
            cli.add_argument(cmd);
        }

    public:
        virtual std::string name() = 0;
        virtual std::string description() = 0;
        virtual void setup(lyra::command &builder) = 0;
        virtual void on_run(const lyra::group &g) = 0;
        virtual void sub_command_setup() {};
        virtual ~base_command() = default;

    protected:
        bool show_help;

    protected:
        void add_subcommands(std::shared_ptr<sub_command> cmd)
        {
            sub_commands.emplace(cmd->name(), cmd);
        }

    private:
        std::map<std::string, std::shared_ptr<sub_command>> sub_commands;
    };
}

#endif // __CLIENT_CORE_BASE_COMMAND__