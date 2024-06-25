#include <core/sessions/staged_session.h>
#include <domain/images/build_command.h>
#include <domain/images/parser.h>
#include <filesystem>
#include <fmt/color.h>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
namespace fs = std::filesystem;
namespace ro = core::operations::request;
using namespace indicators;

namespace domain::images {
build_command::build_command()
  : command("build"), file_path("."), name(""), session(nullptr),
    progress_bar(std::make_unique<ProgressBar>(option::BarWidth{ 50 },
      option::Start{ "[" },
      option::Fill{ "■" },
      option::Lead{ "■" },
      option::Remainder{ "-" },
      option::End{ " ]" },
      option::PostfixText{ "Loading dependency 1/4" },
      option::ForegroundColor{ Color::cyan },
      option::FontStyles{ std::vector<FontStyle>{ FontStyle::bold } }))
{}
void build_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(file_path, "file")
                     .name("-f")
                     .name("--file")
                     .required()
                     .help("name of the description file (default: `PATH/${NAME}file`)"));
  cmd.add_argument(
    lyra::opt(name, "tag").name("-t").name("--tag").required().help("name and optionally a tag (format: 'name:tag')"));
}
void build_command::on_invockation(const lyra::group &g)
{
  // Hide cursor
  show_console_cursor(false);
  session = std::make_unique<staged_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void build_command::on_start()
{
  fs::path description_file_path(file_path);
  std::error_code error;
  if (auto path_exists = fs::exists(description_file_path, error); error || !path_exists) {
    session->disconnect();
    show_console_cursor(true);
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "build error: {}!\n", error.message());
  } else if (auto result = parse_description(description_file_path); result.has_value()) {
    build_order order = create_build_order(result.value(), name, description_file_path);
    auto content = pack_build_order(order);
    session->write(ro::operation::build, ro::target::image, content);
  }
}
void build_command::on_progress(const std::string &feed, float progress)
{
  if (progress > 0 && progress <= 100) {
    progress_bar->set_progress(progress);// all done
  }
  progress_bar->set_option(option::PostfixText{ feed });
}
void build_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  show_console_cursor(true);
  if(is_failure)
  {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  } else 
  {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "import success: {}!\n", message);
  }
  fmt::print(fg(fmt::color::white), "\n");
}
build_command::~build_command() {}
}// namespace domain::images