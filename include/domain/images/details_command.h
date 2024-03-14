#ifndef __CLIENT_DOMAIN_IMAGES_DETAILS_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_DETAILS_IMAGE_COMMAND__

#include <core/commands/sub_command.h>
#include <memory>

namespace spdlog
{
    class logger;
};

namespace domain::images
{
    class details_command : public core::commands::sub_command
    {
    public:
        details_command();
        virtual ~details_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

    private:
        bool verbose;
        bool progress;
        std::string format;
        std::string identifier;
        std::shared_ptr<spdlog::logger> logger;
    };
}

#endif // __CLIENT_DOMAIN_IMAGES_DETAILS_IMAGE_COMMAND__