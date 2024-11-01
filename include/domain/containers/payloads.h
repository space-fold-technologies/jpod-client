#ifndef __JC_DOMAIN_CONTAINERS_PAYLOADS__
#define __JC_DOMAIN_CONTAINERS_PAYLOADS__
#include <msgpack.hpp>
using namespace std::chrono;
namespace domain::containers {
struct container_creation_order
{
  std::string tagged_image;
  std::string name;
  std::map<std::string, std::string> port_map;
  std::map<std::string, std::string> env_vars;
  std::string network_properties;

  MSGPACK_DEFINE(tagged_image, name, port_map, env_vars, network_properties)
};

inline std::vector<uint8_t> pack_container_creation_order(const container_creation_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

struct container_start_order
{
  std::string name;
  std::string user;
  MSGPACK_DEFINE(name, user)
};

inline std::vector<uint8_t> pack_container_start_order(const container_start_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

struct container_term_order
{
  std::string term;

  MSGPACK_DEFINE(term)
};

inline std::vector<uint8_t> pack_container_term_order(const container_term_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

enum class shell_order_type { start_session, terminal_feed, terminal_size };

struct order_properties
{
  std::string name;
  std::vector<std::string> commands;
  bool interactive;
  std::string user;
  uint16_t columns;
  uint16_t rows;
  MSGPACK_DEFINE(name, commands, interactive, user, columns, rows)
};

struct container_shell_order
{
  shell_order_type type;
  std::vector<uint8_t> data;

  MSGPACK_DEFINE(type, data)
};
inline std::vector<uint8_t> pack_container_properties(const order_properties &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}
inline std::vector<uint8_t> pack_container_shell_order(const container_shell_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

enum class filter_mode { active, all, shutdown };

struct container_list_order
{
  filter_mode mode;
  std::string query;

  MSGPACK_DEFINE(mode, query)
};

inline std::vector<uint8_t> pack_container_list_order(const container_list_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

inline auto mode_value(filter_mode mode) -> std::string
{

  if (mode == filter_mode::active)
    return "active";
  else if (mode == filter_mode::shutdown)
    return "shutdown";
  return "all";
}

struct container_summary_entry
{
  std::string identifier;
  std::string name;
  std::string image;
  std::string status;
  std::map<std::string, std::string> port_map;
  time_point<system_clock, nanoseconds> created_at;
  MSGPACK_DEFINE(identifier, name, image, status, port_map, created_at)
};

struct container_summary
{
  std::vector<container_summary_entry> entries;

  MSGPACK_DEFINE(entries)
};

inline container_summary unpack_container_summary(const std::vector<uint8_t> &content)
{
  msgpack::object_handle result;
  msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
  return result.get().as<container_summary>();
}
struct container_remove_order
{
  std::string term;
  bool force;
  MSGPACK_DEFINE(term, force)
};
inline std::vector<uint8_t> pack_container_remove_order(const container_remove_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}
struct container_log_order
{
  std::string name;
  bool follow;
  uint16_t tail;
  bool timestamps;
  MSGPACK_DEFINE(name, follow, tail, timestamps)
};
inline std::vector<uint8_t> pack_container_log_order(const container_log_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}
}// namespace domain::containers
MSGPACK_ADD_ENUM(domain::containers::shell_order_type);
MSGPACK_ADD_ENUM(domain::containers::filter_mode);

#endif//__JC_DOMAIN_CONTAINERS_PAYLOADS__