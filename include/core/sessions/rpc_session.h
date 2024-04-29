#ifndef __JC_CORE_SESSIONS_RPC_SESSION__
#define __JC_CORE_SESSIONS_RPC_SESSION__

#include <core/sessions/session.h>

namespace core::operations {
class rpc_session_listener;
class rpc_session : public session
{
public:
  rpc_session(rpc_session_listener &listener);
  rpc_session() = delete;
  void on_data(response::operation operation, const std::vector<uint8_t> &content) override;
  void on_error(const std::error_code& error) override;
  void on_open() override;
  virtual ~rpc_session();

private:
  rpc_session_listener &listener;
};
}// namespace core::operations

#endif//__JC_CORE_SESSIONS_RPC_SESSION__
