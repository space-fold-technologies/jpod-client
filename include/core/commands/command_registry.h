#ifndef __JC_CORE_COMMANDS_COMMAND_REGISTRY__
#define __JC_CORE_COMMANDS_COMMAND_REGISTRY__

#include <lyra/lyra.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace core::commands
{
    class command;

    using handler_provider = std::function<std::unique_ptr<command>()>;
    class command_registry
    {
    public:
        command_registry();
        virtual ~command_registry();
        void add(const std::string &scope, handler_provider provider);
        void initialize(lyra::cli& cli);

    private:
        std::map<std::string, lyra::command> scopes;
        std::map<std::string, std::vector<std::unique_ptr<command>>> commands;
    };
}
#endif //__JC_CORE_COMMANDS_COMMAND_REGISTRY__