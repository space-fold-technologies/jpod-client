#include <domain/containers/terminal_listener.h>
#include <domain/containers/terminal.h>
#include <asio/io_context.hpp>
#include <asio/write.hpp>
#include <asio/post.hpp>
#include <asio/read.hpp>
#include <sys/ioctl.h>
#include <istream>
#include <thread>
#include <vector>

namespace domain::containers {
terminal::terminal(asio::io_context &context, terminal_listener &listener): 
context(context), 
listener(listener), 
file_descriptor(::dup(STDIN_FILENO)),
in(context.get_executor(), file_descriptor), 
out(context.get_executor(), ::dup(STDOUT_FILENO)),
previous_attributes{}, 
restored(false)
{}

void terminal::initiate()
{
  if (::tcgetattr(file_descriptor, &previous_attributes) == 0) 
  {
    termios current_attributes = previous_attributes;
    cfmakeraw(&current_attributes);
    current_attributes.c_oflag |= OPOST;
    if (::tcsetattr(file_descriptor, TCSANOW, &current_attributes) == 0) 
    {
      #if defined(TIOCGWINSZ)
        winsize size{};
        if (::ioctl(file_descriptor, TIOCGWINSZ, &size) == 0) 
        {
          listener.on_terminal_initialized(size.ws_row, size.ws_col);
        } 
    #elif defined(TIOCGSIZE)
        ttysize size{}
        if(::ioctl(file_descriptor, TIOCGWINSZ, &size) == 0)
        {
          listener.on_terminal_initialized(size.ts_row, size.ts.col);
        }
    #endif
    } 
    else 
    {
      listener.on_terminal_error(std::error_code{errno, std::system_category()});
    }
  }
}
void terminal::start_io()
{
  asio::post([this]() 
  { 
      read_user_input(); 
  });
}

void terminal::restore()
{
  ::tcsetattr(file_descriptor, TCSANOW, &previous_attributes);
  restored = true;
}
void terminal::write(const std::vector<uint8_t> &content)
{
  // new mechanism involving drawing to the screen and tracking the cursors position as well as taking in events from keys and mouse
  // may need to create a terminal emulation library for this case
  // detected keys will be handled from asio capture
  std::string out_buffer(content.begin(), content.end());
  out.async_write_some(
    asio::buffer(out_buffer),
    [this](const std::error_code &err, std::size_t bytes_transferred) 
    {
      if (err) 
      { 
        listener.on_terminal_error(err); 
      }
    });
}
void terminal::read_user_input()
{
  asio::async_read(
    in, 
    input_buffer, 
    asio::transfer_at_least(1), 
    [this](const std::error_code &err, std::size_t bytes_transferred) 
    {
      if (!err) 
      {
        std::istream stream(&input_buffer);
        std::vector<uint8_t> content(bytes_transferred);
        stream.read((char *)&content[0], bytes_transferred);
        input_buffer.consume(input_buffer.size());
        listener.on_input_received(content);
        read_user_input();
      } 
      else 
      {
        listener.on_terminal_error(err);
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

}// namespace domain::containers