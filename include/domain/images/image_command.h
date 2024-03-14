#ifndef __CLIENT_DOMAIN_IMAGES_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_IMAGE_COMMAND__

#include <core/commands/base_command.h>
#include <memory>

namespace spdlog
{
    class logger;
};
namespace domain::images
{
    class image_command : public core::commands::base_command
    {
    public:
        image_command();
        virtual ~image_command();
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

#endif // __CLIENT_DOMAIN_IMAGES_IMAGE_COMMAND__