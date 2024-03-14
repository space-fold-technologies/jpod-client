#ifndef __CLIENT_CORE_COMMAND_REGISTRY__
#define __CLIENT_CORE_COMMAND_REGISTRY__

#include <map>
#include <string>
#include <memory>

namespace lyra
{
    class cli;
};
namespace core::commands
{
    class base_command;
    class command_registry
    {
       
    public:
        command_registry();
        void add(std::shared_ptr<base_command> entry);
        void register_commands(lyra::cli& cli);
    private:
        std::map<std::string, std::shared_ptr<base_command>> command_map;
    };
}

#endif // __CLIENT_CORE_COMMAND_REGISTRY__