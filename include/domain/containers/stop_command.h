#ifndef __JC_DOMAIN_CONTAINERS_STOP_COMMAND__
#define __JC_DOMAIN_CONTAINERS_STOP_COMMAND__

#include <core/commands/command.h>
#include <core/sessions/rpc_session_listener.h>
#include <memory>

namespace core::operations {
class rpc_session;
};
using namespace core::operations;

namespace domain::containers {
class stop_command
  : public core::commands::command
  , public core::operations::rpc_session_listener
{
public:
  stop_command();
  void on_setup(lyra::command &cmd) override;
  void on_invockation(const lyra::group &g) override;
  void on_start() override;
  void on_response(const std::vector<uint8_t> &data) override;
  void on_finish(bool is_failure, const std::string &message) override;
  virtual ~stop_command();

private:
  std::string name;
  std::unique_ptr<rpc_session> session;
};
}// namespace domain::containers
#endif//__JC_DOMAIN_CONTAINERS_STOP_COMMAND__