#ifndef __CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL__
#define __CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL__

#include <string>
#include <map>
#include <cstdint>
#include <termios.h>
#include <memory>
#include <asio/io_context.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <asio/streambuf.hpp>

namespace spdlog
{
    class logger;
};

namespace domain::containers::shells
{
    /*
         might have to create some kind of async I/O for web sockets to read user input and write data to canvas
       */
    constexpr std::size_t default_read_buffer_size = 1024;
    class terminal_listener;
    struct terminal_details
    {
        std::string environment;
        uint32_t rows;
        uint32_t columns;
    };

    class terminal
    {
    public:
        terminal(asio::io_context &context, terminal_listener &listener);
        ~terminal();
        void initiate();
        void start_io();
        terminal_details details();
        bool is_active();
        void restore();
        void write(const std::vector<uint8_t> &content);

    private:
        void read_user_input();
        void wait_to_read();

    private:
        asio::io_context &context;
        terminal_listener &listener;
        int file_descriptor;
        asio::posix::stream_descriptor in;
        asio::posix::stream_descriptor out;
        termios previous_attributes;                                                                         
        bool restored;
        //asio::streambuf input_buffer;
        uint8_t input_buffer[1];
        std::shared_ptr<spdlog::logger> logger;
    };
}
#endif //__CLIENT_DOMAIN_CONTAINERS_SHELL_TERMINAL__