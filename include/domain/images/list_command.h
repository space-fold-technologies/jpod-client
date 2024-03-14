#ifndef __CLIENT_DOMAIN_IMAGES_LIST_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_LIST_IMAGE_COMMAND__

#include <core/commands/sub_command.h>
#include <core/operations/operation_listener.h>
#include <memory>

namespace core::operations
{
    class operation;
};
namespace domain::images
{
    class list_command : public core::commands::sub_command, public core::operations::operation_listener
    {
    public:
        list_command();
        virtual ~list_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

        // callbacks
        void on_operation_started() override;
        void on_operation_data_received(
            core::connections::operation_target target,
            core::connections::response_operation op,
            const std::vector<uint8_t> &payload) override;
        void on_operation_complete(const std::error_code &error, const std::string &details) override;
        void on_progress_update(const std::string& operation, const std::vector<uint8_t>& content) override;   
        void on_operation_success(const std::string& payload) override;

    private:
        std::string query;
        std::unique_ptr<core::operations::operation> operation;
    };
}

#endif // __CLIENT_DOMAIN_IMAGES_LIST_IMAGE_COMMAND__