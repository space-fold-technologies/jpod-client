#ifndef __JC_CORE_SESSIONS_ASYNC_SESSION_LISTENER__
#define __JC_CORE_SESSIONS_ASYNC_SESSION_LISTENER__
#include <string>
#include <cstdint>
#include <vector>

namespace core::operations
{
    class interactive_session_listener
    {
        public:
        virtual void on_start(bool is_remote) = 0;
        virtual void on_data_received(const std::vector<uint8_t>& data) = 0;
        virtual void on_finish(bool is_failure, const std::string &message) = 0;
    };
}
#endif //__JC_CORE_SESSIONS_ASYNC_SESSION_LISTENER__