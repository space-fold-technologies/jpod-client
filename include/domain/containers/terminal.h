#ifndef __JC_DOMAIN_CONTAINERS_TERMINAL__
#define __JC_DOMAIN_CONTAINERS_TERMINAL__

#include <asio/posix/stream_descriptor.hpp>
#include <asio/streambuf.hpp>
#include <termios.h>
#include <cstdint>
#include <memory>
#include <string>
#include <map>


namespace asio
{
  class io_context;
};

namespace domain::containers {
constexpr std::size_t default_read_buffer_size = 1024;
class terminal_listener;

class terminal
{
public:
  terminal(asio::io_context &context, terminal_listener &listener);
  ~terminal();
  void initiate();
  void start_io();
  void write(const std::vector<uint8_t> &content);

private:
  void read_user_input();
  void restore();

private:
  asio::io_context &context;
  terminal_listener &listener;
  int file_descriptor;
  asio::posix::stream_descriptor in;
  asio::posix::stream_descriptor out;
  termios previous_attributes;
  bool restored;
  asio::streambuf input_buffer;
  
};
}// namespace domain::containers
#endif//__JC_DOMAIN_CONTAINERS_TERMINAL__