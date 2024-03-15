#include <domain/containers/shell/terminal.h>
#include <domain/containers/shell/terminal_listener.h>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <asio/post.hpp>
#include <sys/ioctl.h>
#include <vector>
#include <thread>
#include <istream>
#include <spdlog/spdlog.h>

namespace domain::containers::shells
{
    terminal::terminal(asio::io_context &context, terminal_listener &listener) : context(context),
                                                                                 listener(listener),
                                                                                 file_descriptor(::dup(STDIN_FILENO)),
                                                                                 in(context.get_executor(), file_descriptor),
                                                                                 out(context.get_executor(), ::dup(STDOUT_FILENO)),
                                                                                 previous_attributes{},
                                                                                 restored(false),
                                                                                 logger(spdlog::get("jpod"))

    {
    }

    void terminal::initiate()
    {
        if (::tcgetattr(file_descriptor, &previous_attributes) == 0)
        {
            termios current_attributes = previous_attributes;
            current_attributes.c_lflag &= ~ (ICANON | ECHO);
            // current_attributes.c_iflag &= tcflag_t(~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON));
            // current_attributes.c_oflag &= tcflag_t(~(OPOST));
            // current_attributes.c_lflag &= tcflag_t(~(ECHO | ECHONL | ICANON | ISIG | IEXTEN));
            // current_attributes.c_cflag &= tcflag_t(~(CSIZE | PARENB));
            // current_attributes.c_cflag |= tcflag_t(~(CS8));
            // current_attributes.c_cc[VMIN] = 1;
            // current_attributes.c_cc[VTIME] = 0;
            if (::tcsetattr(file_descriptor, TCSANOW, &current_attributes) == 0)
            {
                listener.on_terminal_initialized();
            }
            else
            {
                this->listener.on_terminal_error(std::make_error_code(std::errc::bad_file_descriptor));
            }
        }
    }
    void terminal::start_io()
    {
        asio::post([this]()
                   { read_user_input(); });
    }
    terminal_details terminal::details()
    {
        winsize size{};
        terminal_details details{};
        if (ioctl(file_descriptor, TIOCGWINSZ, &size) < 0)
        {
            // failed to get window size
        }
        details.columns = size.ws_col;
        details.rows = size.ws_row;
        return details;
    }
    bool terminal::is_active()
    {
        return in.is_open() && out.is_open();
    }
    void terminal::restore()
    {
        ::tcsetattr(file_descriptor, TCSANOW, &previous_attributes);
        restored = true;
    }
    void terminal::write(const std::vector<uint8_t> &content)
    {
        in.async_write_some(
            asio::buffer(std::string(content.begin(), content.end())),
            [this](const std::error_code &err, std::size_t bytes_transferred)
            {
                if (err)
                {
                    this->listener.on_terminal_error(err);
                }
            });
    }
    void terminal::read_user_input()
    {
        asio::async_read_until(
            in,
            input_buffer,
            "\n",
            [this](const std::error_code &err, std::size_t bytes_transferred)
            {
                if (!err)
                {
                    std::istream stream(&this->input_buffer);
                    std::vector<uint8_t> content(bytes_transferred);
                    stream.read((char *)&content[0], bytes_transferred);
                    this->listener.on_input_received(content);
                    this->input_buffer.consume(bytes_transferred);
                    this->read_user_input();
                }
                else
                {
                    this->listener.on_terminal_error(err);
                }
            });
    }
    void terminal::wait_to_read()
    {
        in.async_wait(
            asio::posix::stream_descriptor::wait_read,
            [this](const std::error_code &err)
            {
                if (!err)
                {
                    this->logger->info("ready to read input");
                    this->read_user_input();
                }
                else
                {
                    this->listener.on_terminal_error(err);
                }
            });
    }
    terminal::~terminal()
    {
        if (!restored)
        {
            restore();
        }
        if (file_descriptor > 0)
        {
            close(file_descriptor);
        }
    }
}