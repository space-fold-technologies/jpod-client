#ifndef __JC_CORE_SESSIONS_STAGED_SESSION_LISTENER__
#define __JC_CORE_SESSIONS_STAGED_SESSION_LISTENER__
#include <cstdint>
#include <string>
#include <vector>

namespace core::operations {
class staged_session_listener
{
public:
  virtual void on_start() = 0;
  virtual void on_progress(const std::string &feed, float progress) = 0;
  virtual void on_finish(bool is_failure, const std::string &message) = 0;
};

}// namespace core::operations

#endif//__JC_CORE_SESSIONS_STAGED_SESSION_LISTENER__