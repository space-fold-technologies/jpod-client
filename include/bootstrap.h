#ifndef __JC_BOOTSTRAP__
#define __JC_BOOTSTRAP__

#include <core/commands/command_registry.h>
#include <domain/containers/list_command.h>
#include <domain/containers/remove_command.h>
#include <domain/containers/create_command.h>
#include <domain/containers/shell_command.h>
#include <domain/containers/start_command.h>
#include <domain/containers/stop_command.h>
#include <domain/containers/log_command.h>
#include <domain/images/import_command.h>
#include <domain/images/list_command.h>
#include <domain/images/build_command.h>
#include <domain/images/pull_command.h>

namespace cc = core::commands;
namespace di = domain::images;
namespace dc = domain::containers;
using cmd_ptr = std::unique_ptr<cc::command>;
inline auto setup_commands(cc::command_registry &registry) -> void
{
  registry.add("image", []() -> cmd_ptr { return std::make_unique<di::list_command>(); });
  registry.add("image", []() -> cmd_ptr { return std::make_unique<di::build_command>(); });
  registry.add("image", []() -> cmd_ptr { return std::make_unique<di::import_command>(); });
  registry.add("image", []() -> cmd_ptr { return std::make_unique<di::pull_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::create_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::list_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::start_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::stop_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::shell_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::remove_command>(); });
  registry.add("container", []() -> cmd_ptr { return std::make_unique<dc::log_command>(); });
}

#endif//__JC_BOOTSTRAP__