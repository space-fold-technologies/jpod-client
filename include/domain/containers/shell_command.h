#ifndef __CLIENT_DOMAIN_CONTAINERS_SHELL_CONTAINER_COMMAND__
#define __CLIENT_DOMAIN_CONTAINERS_SHELL_CONTAINER_COMMAND__

#include <core/commands/sub_command.h>
#include <core/operations/operation_listener.h>
#include <domain/containers/shell/terminal_listener.h>
#include <memory>
#include <map>

namespace core::operations
{
    class operation;
};

namespace domain::containers::shells
{
    class terminal;
};

namespace domain::containers
{
    class shell_command : public core::commands::sub_command, public core::operations::operation_listener, public shells::terminal_listener
    {
    public:
        shell_command();
        virtual ~shell_command();
        std::string name() override;
        std::string description() override;
        void setup(lyra::command &builder) override;
        void on_run(const lyra::group &g) override;

        // operation callbacks
        void on_operation_started() override;
        void on_operation_data_received(
            core::connections::operation_target target,
            core::connections::response_operation operation,
            const std::vector<uint8_t> &payload) override;
        void on_operation_complete(const std::error_code &error, const std::string &details) override;
        void on_progress_update(const std::string &operation, const std::vector<uint8_t> &content) override;
        void on_operation_success(const std::string &payload) override;
        // terminal callbacks
        void on_input_received(const std::vector<uint8_t> &content) override;
        void on_terminal_initialized() override;
        void on_terminal_resized(uint32_t rows, uint32_t columns) override;
        void on_terminal_error(const std::error_code &err) override;

    private:
        std::string container_name;
        std::unique_ptr<core::operations::operation> operation;
        std::unique_ptr<shells::terminal> terminal;
    };
}
#endif // __CLIENT_DOMAIN_CONTAINERS_SHELL_CONTAINER_COMMAND__