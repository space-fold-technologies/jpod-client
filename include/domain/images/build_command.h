#ifndef __JC_DOMAIN_IMAGES_BUILD_COMMAND__
#define __JC_DOMAIN_IMAGES_BUILD_COMMAND__

#include <core/commands/command.h>
#include <core/sessions/staged_session_listener.h>
#include <memory>
#include <optional>

namespace core::operations {
class staged_session;
};
namespace indicators {
class ProgressBar;
}
using namespace core::operations;
namespace domain::images {
class build_command
  : public core::commands::command
  , public core::operations::staged_session_listener
{
public:
  build_command();
  void on_setup(lyra::command &cmd) override;
  void on_invockation(const lyra::group &g) override;
  // callback listeners
  void on_start() override;
  void on_progress(const std::string &feed, float progress) override;
  void on_finish(bool is_failure, const std::string &message) override;
  virtual ~build_command();

private:
  std::string file_path;
  std::string name;
  std::unique_ptr<staged_session> session;
  std::unique_ptr<indicators::ProgressBar> progress_bar;
};
}// namespace domain::images

#endif//__JC_DOMAIN_IMAGES_BUILD_COMMAND__