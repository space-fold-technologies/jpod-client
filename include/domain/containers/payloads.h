#ifndef __CLIENT_DOMAIN_CONTAINERS_CONTAINER_PAYLOADS__
#define __CLIENT_DOMAIN_CONTAINERS_CONTAINER_PAYLOADS__

#include <msgpack.hpp>
using namespace std::chrono;

namespace domain::containers
{
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

    namespace shell
    {
        constexpr uint8_t start_session = 0x00;
        constexpr uint8_t terminal_size = 0x01;
        constexpr uint8_t terminal_feed = 0x02;
    }
    struct container_shell_order
    {
        uint8_t type;
        std::vector<uint8_t> data;

        MSGPACK_DEFINE(type, data)
    };

    inline std::vector<uint8_t> pack_container_shell_order(const container_shell_order &order)
    {
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, order);
        std::vector<uint8_t> output(buffer.size());
        std::memcpy(output.data(), buffer.data(), buffer.size());
        return output;
    }

    namespace filter
    {
        constexpr uint8_t all = 0x00;
        constexpr uint8_t active = 0x01;
        constexpr uint8_t shutdown = 0x02;
    };
    struct container_list_order
    {
        uint8_t mode;
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

    inline auto mode_value(uint8_t mode) -> std::string
    {

        if (mode == filter::active)
            return "active";
        else if (mode == filter::shutdown)
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

    inline container_summary unpack_container_summary(const std::vector<uint8_t>& content)
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
}
#endif // __CLIENT_DOMAIN_CONTAINERS_CONTAINER_PAYLOADS__