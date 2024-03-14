#include <cstdlib>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <core/commands/command_registry.h>
#include <domain/images/image_command.h>
#include <domain/containers/container_commands.h>
#if defined __has_include
#if __has_include("spdlog/fmt/bundled/core.h")
#error "bundled fmt within spdlog should not be present"
#endif
#endif

using namespace core::commands;
using namespace domain::images;
using namespace domain::containers;

auto main(int argc, char *argv[]) -> int
{
  auto console = spdlog::stdout_color_mt("jpod");
  auto cli = lyra::cli();
  bool show_help = false;
  cli.add_argument(lyra::help(show_help));
  auto cmd_reg = std::make_unique<command_registry>();
  cmd_reg->add(std::make_shared<image_command>());
  cmd_reg->add(std::make_shared<container_command>());
  cmd_reg->register_commands(cli);
  auto result = cli.parse({argc, argv});
  if (show_help)
  {
    std::cout << cli;
    return 0;
  }
  if (!result)
  {
    std::cerr << result.message() << "\n";
  }
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}