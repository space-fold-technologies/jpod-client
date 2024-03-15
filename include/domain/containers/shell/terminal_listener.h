#ifndef __CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL_LISTENER__
#define __CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL_LISTENER__
#include <vector>
#include <cstdint>
#include <system_error>

namespace domain::containers::shells
{
    class terminal_listener
    {
    public:
        virtual void on_input_received(const std::vector<uint8_t> &content) = 0;
        virtual void on_terminal_initialized() = 0;
        virtual void on_terminal_resized(uint32_t rows, uint32_t columns) = 0;
        virtual void on_terminal_error(const std::error_code &err) = 0;
    };

}

#endif // __CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL_LISTENER__