#ifndef __CLIENT_DOMAIN_CONTAINERS_CONTAINER_COMMANDS__
#define __CLIENT_DOMAIN_CONTAINERS_CONTAINER_COMMANDS__
#include <core/commands/base_command.h>
#include <memory>

namespace spdlog
{
    class logger;
};
namespace domain::containers
{
    class container_command : public core::commands::base_command
    {
    public:
        container_command();
        virtual ~container_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;
        void sub_command_setup() override;

    private:
        bool verbose;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_DOMAIN_CONTAINERS_CONTAINER_COMMANDS__