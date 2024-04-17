#ifndef __JC_CORE_COMMANDS_COMMAND__
#define __JC_CORE_COMMANDS_COMMAND__

#include <lyra/lyra.hpp>

namespace core::commands {
class command
{
public:
  command(const std::string &name)
    : cmd(name, [this](const lyra::group &g) { this->on_invockation(g); })
  {
  }

  virtual ~command() = default;
  virtual void on_setup(lyra::command &cmd) = 0;
  virtual void on_invockation(const lyra::group &g) = 0;
  void initialize(lyra::command &scope)
  {
    this->on_setup(cmd);

    scope.add_argument(cmd);
  }

private:
    lyra::command cmd;
};
}// namespace core::commands
#endif//__JC_CORE_COMMANDS_COMMAND__