#ifndef __JC_CORE_SESSIONS_DETAILS__
#define __JC_CORE_SESSIONS_DETAILS__
#include <cstdint>

namespace core::operations::request {
enum class operation : std::uint8_t {
  list = 0x00,
  details = 0x01,
  removal = 0x02,
  push = 0x03,
  pull = 0x04,
  build = 0x05,
  import = 0x06,
  authorize = 0x07,
  start = 0x08,
  stop = 0x09,
  shell = 0x0A,
  logs = 0x0B
};
enum class target : std::uint8_t {
  image = 0b00000000,
  container = 0b00100000,
  registry = 0b01000000,
  plugin = 0b01100000,
  client = 0b10000000
};
};// namespace core::operations::request

namespace core::operations::response {
enum class operation : std::uint8_t {
  success = 0x00,
  failure = 0x01,
  progress = 0x02,
  close_order = 0x03,
  frame = 0x04
};
};// namespace core::operations::response
#endif//__JC_CORE_SESSIONS_DETAILS__