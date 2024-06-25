#ifndef __JC_CORE_SESSIONS_RPC_SESSION_LISTENER__
#define __JC_CORE_SESSIONS_RPC_SESSION_LISTENER__
#include <string>
#include <cstdint>
#include <vector>

namespace core::operations
{
    class rpc_session_listener
    {
        public:
        virtual void on_start() = 0;
        virtual void on_response(const std::vector<uint8_t>& data) = 0;
        virtual void on_failure(const std::string &message) = 0;
    };
}
#endif //__JC_CORE_SESSIONS_RPC_SESSION_LISTENER__