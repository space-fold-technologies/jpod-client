#ifndef __JC_DOMAIN_CONTAINERS_TERMINAL_LISTENER__
#define __JC_DOMAIN_CONTAINERS_TERMINAL_LISTENER__

#include <cstdint>
#include <system_error>
#include <vector>

namespace domain::containers {
class terminal_listener
{
public:
  virtual void on_input_received(const std::vector<uint8_t> &content) = 0;
  virtual void on_terminal_initialized(uint32_t rows, uint32_t columns) = 0;
  virtual void on_terminal_resized(uint32_t rows, uint32_t columns) = 0;
  virtual void on_terminal_error(const std::error_code &err) = 0;
};
}// namespace domain::containers
#endif//__JC_DOMAIN_CONTAINERS_TERMINAL_LISTENER__