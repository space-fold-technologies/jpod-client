#ifndef __CLIENT_DOMAIN_IMAGES_IMAGE_PAYLOADS__
#define __CLIENT_DOMAIN_IMAGES_IMAGE_PAYLOADS__

#include <msgpack.hpp>
using namespace std::chrono;

namespace domain::images
{
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

    enum class step_type : int
    {
        from = 0,
        run = 1,
        work_dir = 2,
        copy = 3
    };

    struct stage
    {
        std::string name;
        std::string tag;
        std::map<std::string, std::string> labels;
        std::map<std::string, step_type> steps;

        MSGPACK_DEFINE(name, tag, labels, steps)

        bool operator==(const stage rhs)
        {
            return (this->name == rhs.name && this->tag == rhs.tag && this->labels == rhs.labels && this->steps == rhs.steps);
        }
    };

    struct build_order
    {
        std::string name;
        std::string tag;
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

    struct progress_frame
    {
        std::string entry_name;
        std::string sub_entry_name;
        std::string feed;
        uint8_t percentage;
        MSGPACK_DEFINE(entry_name, sub_entry_name, feed, percentage)
    };

    inline progress_frame unpack_progress_frame(const std::vector<uint8_t> &content)
    {
        msgpack::object_handle result;
        msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
        return result.get().as<progress_frame>();
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
}
MSGPACK_ADD_ENUM(domain::images::step_type);
#endif // __CLIENT_DOMAIN_IMAGES_IMAGE_PAYLOADS__