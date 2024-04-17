#ifndef __JC_DOMAIN_CONTAINERS_LOG_COMMAND__
#define __JC_DOMAIN_CONTAINERS_LOG_COMMAND__

#include <core/commands/command.h>
#include <core/sessions/interactive_session_listener.h>
#include <memory>

namespace core::operations {
class interactive_session;
};

using namespace core::operations;

namespace domain::containers {
class terminal;
class log_command
  : public core::commands::command
  , public core::operations::interactive_session_listener
{
public:
  log_command();
  void on_setup(lyra::command &cmd) override;
  void on_invockation(const lyra::group &g) override;
  // interactive session callbacks
  void on_start(bool is_remote) override;
  void on_data_received(const std::vector<uint8_t> &data) override;
  void on_finish(bool is_failure, const std::string &message) override;
  virtual ~log_command();

private:
  std::string name;
  bool follow;
  uint16_t tail;
  bool timestamps;
  std::unique_ptr<interactive_session> session;
};
}// namespace domain::containers
#endif//__JC_DOMAIN_CONTAINERS_LOG_COMMAND__