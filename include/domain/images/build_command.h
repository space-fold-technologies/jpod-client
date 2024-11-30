#ifndef __JC_DOMAIN_IMAGES_BUILD_COMMAND__
#define __JC_DOMAIN_IMAGES_BUILD_COMMAND__

#include <core/commands/command.h>
#include <core/sessions/interactive_session_listener.h>
#include <memory>
#include <optional>

namespace core::operations {
class interactive_session;
};

using namespace core::operations;
namespace domain::images {
class build_command: 
public core::commands::command,
public core::operations::interactive_session_listener
{
public:
  build_command();
  void on_setup(lyra::command &cmd) override;
  void on_invockation(const lyra::group &g) override;
  // interactive session callbacks
  void on_start(bool is_remote) override;
  void on_data_received(const std::vector<uint8_t> &data) override;
  void on_finish(bool is_failure, const std::string &message) override;
  virtual ~build_command();

private:
  std::string file_path;
  std::string name;
  std::unique_ptr<interactive_session> session;
};
}// namespace domain::images

#endif//__JC_DOMAIN_IMAGES_BUILD_COMMAND__