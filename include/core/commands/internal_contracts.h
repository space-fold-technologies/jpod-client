#ifndef __CLIENT_CORE_COMMANDS_INTERNAL_CONTRACTS__
#define __CLIENT_CORE_COMMANDS_INTERNAL_CONTRACTS__

#include <msgpack.hpp>

namespace core::commands
{
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

    struct progress_payload
    {
        std::string operation;
        std::vector<uint8_t> content;
        MSGPACK_DEFINE(operation, content);
    };

    inline progress_payload unpack_progress_payload(const std::vector<uint8_t> &content)
    {
        msgpack::object_handle result;
        msgpack::unpack(result, reinterpret_cast<const char *>(content.data()), content.size());
        return result.get().as<progress_payload>();
    }
}
#endif // __CLIENT_CORE_COMMANDS_INTERNAL_CONTRACTS__