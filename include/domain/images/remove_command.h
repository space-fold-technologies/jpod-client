#ifndef __CLIENT_DOMAIN_IMAGES_REMOVE_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_REMOVE_IMAGE_COMMAND__

#include <core/commands/sub_command.h>
#include <memory>

namespace spdlog
{
    class logger;
};

namespace domain::images
{
    class remove_command : public core::commands::sub_command
    {
    public:
        remove_command();
        virtual ~remove_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

    private:
        bool verbose;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_DOMAIN_IMAGES_REMOVE_IMAGE_COMMAND__