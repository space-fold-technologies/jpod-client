#ifndef __CLIENT_DOMAIN_IMAGES_IMPORT_IMAGE_COMMAND__
#define __CLIENT_DOMAIN_IMAGES_IMPORT_IMAGE_COMMAND__

#include <core/operations/operation_listener.h>
#include <core/commands/sub_command.h>
#include <memory>

namespace core::operations
{
    class operation;
};

namespace cn = core::connections;
namespace domain::images
{
    class import_command : public core::commands::sub_command, public core::operations::operation_listener
    {
    public:
        import_command();
        virtual ~import_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;
        void on_operation_started() override;
        void on_operation_data_received(
            cn::operation_target target,
            cn::response_operation operation,
            const std::vector<uint8_t> &payload) override;
        void on_operation_success(const std::string &payload) override;
        void on_operation_complete(const std::error_code &error, const std::string &details) override;
        void on_progress_update(const std::string &operation, const std::vector<uint8_t> &content) override;

    private:
        std::string file_path;
        std::string format;
        std::unique_ptr<core::operations::operation> operation;
    };
}
#endif // __CLIENT_DOMAIN_IMAGES_IMPORT_IMAGE_COMMAND__