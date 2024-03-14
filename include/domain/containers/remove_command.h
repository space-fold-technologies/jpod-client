#ifndef __CLIENT_DOMAIN_CONTAINERS_REMOVE_CONTAINER_COMMAND__
#define __CLIENT_DOMAIN_CONTAINERS_REMOVE_CONTAINER_COMMAND__

#include <core/commands/sub_command.h>
#include <core/operations/operation_listener.h>
#include <memory>
#include <map>

namespace core::operations
{
    class operation;
};
namespace domain::containers
{
    class remove_command : public core::commands::sub_command, public core::operations::operation_listener
    {
    public:
        remove_command();
        virtual ~remove_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

        // callbacks
        void on_operation_started() override;
        void on_operation_data_received(
            core::connections::operation_target target,
            core::connections::response_operation operation,
            const std::vector<uint8_t> &payload) override;
        void on_operation_complete(const std::error_code &error, const std::string &details) override;
        void on_progress_update(const std::string &operation, const std::vector<uint8_t> &content) override;
        void on_operation_success(const std::string &payload) override;

    private:
        std::string container_name;
        bool force;
        std::unique_ptr<core::operations::operation> operation;
    };
}

#endif //__CLIENT_DOMAIN_CONTAINERS_REMOVE_CONTAINER_COMMAND__