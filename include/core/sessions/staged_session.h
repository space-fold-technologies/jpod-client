#ifndef __JC_CORE_SESSIONS_STAGED_SESSION__
#define __JC_CORE_SESSIONS_STAGED_SESSION__

#include <core/sessions/session.h>

namespace core::operations {
class staged_session_listener;

class staged_session : public session
{
public:
  staged_session(staged_session_listener &listener);
  staged_session() = delete;
  void on_data(response::operation operation, const std::vector<uint8_t> &content) override;
  void on_error(const std::error_code& error) override;
  void on_open() override;
  virtual ~staged_session();

private:
  staged_session_listener &listener;
};
}// namespace core::operations

#endif//__JC_CORE_SESSIONS_STAGED_SESSION__
