#ifndef __CLIENT_CORE_OPERATIONS_OPERATION_LISTENER__
#define __CLIENT_CORE_OPERATIONS_OPERATION_LISTENER__
#include <vector>
#include <cstdint>
#include <system_error>
#include <core/connections/frame.h>

namespace core::operations
{
    class operation_listener
    {
    public:
        virtual void on_operation_started() = 0;
        virtual void on_operation_data_received(
            core::connections::operation_target target, 
            core::connections::response_operation operation, 
            const std::vector<uint8_t> &payload) = 0;
        virtual void on_operation_success(const std::string& payload) = 0;
        virtual void on_progress_update(const std::string& operation, const std::vector<uint8_t>& content) = 0;    
        virtual void on_operation_complete(const std::error_code &error, const std::string &details) = 0;
    };
}

#endif // __CLIENT_CORE_OPERATIONS_OPERATION_LISTENER__