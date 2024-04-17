#include <core/sessions/staged_session.h>
#include <domain/images/import_command.h>
#include <domain/images/payloads.h>
#include <filesystem>
#include <fmt/color.h>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>

namespace fs = std::filesystem;
namespace ro = core::operations::request;
using namespace indicators;
namespace domain::images {
import_command::import_command()
  : command("import"), file_path("."), format("afs"), session(nullptr),
    progress_bar(std::make_unique<ProgressBar>(option::BarWidth{ 50 },
      option::Start{ "[" },
      option::Fill{ "■" },
      option::Lead{ "■" },
      option::Remainder{ "-" },
      option::End{ " ]" },
      option::PostfixText{ "" },
      option::ForegroundColor{ Color::cyan },
      option::FontStyles{ std::vector<FontStyle>{ FontStyle::bold } }))
{}
void import_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(file_path, "archive").name("--archive-path").required().help("Image Archive to import."));
  cmd.add_argument(
    lyra::opt(format, "format").name("--format").required().help("Format of the Image to be imported [oci, afs]."));
}
void import_command::on_invockation(const lyra::group &g)
{
  // Hide cursor
  session = std::make_unique<staged_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void import_command::on_start()
{
  fs::path archive_path(file_path);
  std::error_code error;
  if (auto path_exists = fs::exists(archive_path, error); error || !path_exists) {
    session->disconnect();
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "import error: {}!\n", error.message());
  } else {
    auto content = pack_import_order(import_order{ fs::absolute(archive_path).generic_string() });
    session->write(ro::operation::import, ro::target::image, content);
    show_console_cursor(true);
  }
}
void import_command::on_progress(const std::string &feed, float progress)
{
  if (progress > 0 && progress <= 100) {
    progress_bar->set_progress(progress);// all done
  }
  progress_bar->set_option(option::PostfixText{ feed });
}
void import_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  show_console_cursor(true);
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "import error: {}!\n", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "import success: {}!\n", message);
  }
  fmt::print(fg(fmt::color::white), "\n");
}
import_command::~import_command() {}
}// namespace domain::images