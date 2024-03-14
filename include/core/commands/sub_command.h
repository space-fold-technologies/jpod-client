#ifndef __CLIENT_CORE_SUB_COMMAND__
#define __CLIENT_CORE_SUB_COMMAND__

#include <lyra/lyra.hpp>
#include <memory>
#
namespace core::commands
{
    class base_command;
    class sub_command
    {
        friend base_command;
    private:
        void initialize(lyra::command &command)
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
                        this->on_run(g);
                    }
                });
            cmd.help(description()).add_argument(lyra::help(show_help));
            setup(cmd);
            command.add_argument(cmd);
        }

    public:
        virtual ~sub_command() = default;
        virtual std::string name() = 0;
        virtual std::string description() = 0;
        virtual void setup(lyra::command &builder) = 0;
        virtual void on_run(const lyra::group &g) = 0;

    protected:
        bool show_help;
    };
}

#endif // __CLIENT_CORE_SUB_COMMAND__