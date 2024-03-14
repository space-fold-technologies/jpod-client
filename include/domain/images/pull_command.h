#ifndef __CLIENT_DOMAIN_IMAGES_PULL_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_PULL_IMAGE_COMMAND__

#include <core/commands/sub_command.h>
#include <memory>

namespace spdlog
{
    class logger;
};

namespace domain::images
{
    class pull_command : public core::commands::sub_command
    {
    public:
        pull_command();
        virtual ~pull_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

    private:
        bool verbose;
        bool progress;
        std::string tagged_name;
        std::string registry;
        std::string token;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_DOMAIN_IMAGES_PULL_IMAGE_COMMAND__