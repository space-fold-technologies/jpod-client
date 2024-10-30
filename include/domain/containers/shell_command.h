#ifndef __JC_DOMAIN_CONTAINERS_SHELL_COMMAND__
#define __JC_DOMAIN_CONTAINERS_SHELL_COMMAND__

#include <core/commands/command.h>
#include <core/sessions/interactive_session_listener.h>
#include <domain/containers/terminal_listener.h>
#include <map>
#include <memory>

namespace core::operations {
class interactive_session;
};

using namespace core::operations;

namespace domain::containers {
class terminal;
class shell_command
  : public core::commands::command
  , public core::operations::interactive_session_listener
  , public terminal_listener
{
public:
  shell_command();
  void on_setup(lyra::command &cmd) override;
  void on_invockation(const lyra::group &g) override;
  // interactive session callbacks
  void on_start(bool is_remote) override;
  void on_data_received(const std::vector<uint8_t> &data) override;
  void on_finish(bool is_failure, const std::string &message) override;
  // terminal callbacks
  void on_input_received(const std::vector<uint8_t> &content) override;
  void on_terminal_initialized(uint32_t rows, uint32_t columns) override;
  void on_terminal_resized(uint32_t rows, uint32_t columns) override;
  void on_terminal_error(const std::error_code &err) override;
  virtual ~shell_command();

private:
  std::string name;
  bool interactive;
  std::string user;
  std::vector<std::string> commands;
  std::unique_ptr<interactive_session> session;
  std::unique_ptr<terminal> forwarded_terminal;
};
}// namespace domain::containers
#endif//__JC_DOMAIN_CONTAINERS_SHELL_COMMAND__