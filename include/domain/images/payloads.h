#ifndef __JC_DOMAIN_IMAGES_PAYLOADS__
#define __JC_DOMAIN_IMAGES_PAYLOADS__
#include <msgpack.hpp>
using namespace std::chrono;
namespace domain::images {
struct image_term_order
{
  std::string term;
  MSGPACK_DEFINE(term)
};

inline std::vector<uint8_t> pack_image_term_order(const image_term_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
};

struct summary_entry
{
  std::string identifier;
  std::string name;
  std::string repository;
  std::string tag;
  std::size_t size;
  time_point<system_clock, nanoseconds> created_at;
  MSGPACK_DEFINE(identifier, name, repository, tag, size, created_at)
};

struct summary
{
  std::vector<summary_entry> entries;
  MSGPACK_DEFINE(entries)
};

inline summary unpack_summary(const std::vector<uint8_t> &content)
{
  msgpack::object_handle result;
  msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
  return result.get().as<summary>();
}

enum class step_type : int { from = 0, run = 1, work_dir = 2, copy = 3, expose = 4 };

struct stage
{
  std::string name;
  std::map<std::string, step_type> steps;
  MSGPACK_DEFINE(name, steps)

  bool operator==(const stage rhs) { return (this->name == rhs.name && this->steps == rhs.steps); }
};

struct build_order
{
  std::string name;
  std::string tag;
  std::map<std::string, std::string> labels;
  std::string current_directory;
  std::vector<stage> stages;
  std::string entry_point;

  MSGPACK_DEFINE(name, tag, current_directory, stages, entry_point)
};

inline std::vector<uint8_t> pack_build_order(const build_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
}

struct import_order
{
  std::string archive_path;

  MSGPACK_DEFINE(archive_path)
};
inline std::vector<uint8_t> pack_import_order(const import_order &order)
{
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, order);
  std::vector<uint8_t> output(buffer.size());
  std::memcpy(output.data(), buffer.data(), buffer.size());
  return output;
};
}// namespace domain::images
MSGPACK_ADD_ENUM(domain::images::step_type);
#endif//__JC_DOMAIN_IMAGES_PAYLOADS__