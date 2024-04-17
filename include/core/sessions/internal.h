#ifndef __JC_CORE_SESSIONS_INTERNAL__
#define __JC_CORE_SESSIONS_INTERNAL__

#include <msgpack.hpp>

namespace core::operations {
struct success_payload
{
  std::string message;
  MSGPACK_DEFINE(message);
};

inline success_payload unpack_success_payload(const std::vector<uint8_t> &content)
{
  msgpack::object_handle result;
  msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
  return result.get().as<success_payload>();
}

struct error_payload
{
  std::string message;
  MSGPACK_DEFINE(message);
};

inline error_payload unpack_error_payload(const std::vector<uint8_t> &content)
{
  msgpack::object_handle result;
  msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
  return result.get().as<error_payload>();
}

struct progress_frame
{
  std::string feed;
  float percentage;
  MSGPACK_DEFINE(feed, percentage)
};

inline progress_frame unpack_progress_frame(const std::vector<uint8_t> &content)
{
  msgpack::object_handle result;
  msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
  return result.get().as<progress_frame>();
}
}// namespace core::operations

#endif//__JC_CORE_SESSIONS_INTERNAL__