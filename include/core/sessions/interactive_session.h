#ifndef __JC_CORE_SESSIONS_INTERACTIVE_SESSION__
#define __JC_CORE_SESSIONS_INTERACTIVE_SESSION__
#include <core/sessions/session.h>

namespace core::operations {
class interactive_session_listener;
class interactive_session : public session
{
public:
  interactive_session(interactive_session_listener &listener);
  interactive_session() = delete;
  void on_data(response::operation operation, const std::vector<uint8_t> &content) override;
  void on_error(const std::error_code& error) override;
  void on_open() override;
  asio::io_context& context(); 
  virtual ~interactive_session();
  
private:
  interactive_session_listener &listener;
};
}// namespace core::operations

#endif//__JC_CORE_SESSIONS_INTERACTIVE_SESSION__
