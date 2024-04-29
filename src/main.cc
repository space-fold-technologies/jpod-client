#include <bootstrap.h>
#include <cstdlib>
#include <fmt/color.h>
#include <lyra/lyra.hpp>
using namespace core::commands;

int main(int argc, const char **argv)
{
  auto cli = lyra::cli();
  auto registry = std::make_unique<command_registry>();
  setup_commands(*registry);
  registry->initialize(cli);
  auto result = cli.parse({ argc, argv });
  if (!result) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "Error in command line: {}!\n", result.message());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}