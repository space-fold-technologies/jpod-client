#include <core/commands/command.h>
#include <core/commands/command_registry.h>

namespace core::commands {
command_registry::command_registry() : commands{} {}

void command_registry::add(const std::string &scope, handler_provider provider)
{
  if (scopes.find(scope) == scopes.end()) { scopes.try_emplace(scope, lyra::command(scope)); }
  if (commands.find(scope) == commands.end()) { commands.try_emplace(scope, std::vector<std::unique_ptr<command>>{}); }
  if (auto pos = scopes.find(scope); pos != scopes.end()) {
    auto command = provider();
    command->initialize(pos->second);
    commands.at(scope).push_back(std::move(command));
  }
}
void command_registry::initialize(lyra::cli &cli)
{
  std::for_each(scopes.begin(), scopes.end(), [&cli](const auto &entry) { cli.add_argument(entry.second); });
}
command_registry::~command_registry()
{
  commands.clear();
  scopes.clear();
}
}// namespace core::commands