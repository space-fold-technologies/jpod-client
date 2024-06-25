#include <core/sessions/staged_session.h>
#include <domain/images/pull_command.h>
#include <domain/images/payloads.h>
#include <fmt/color.h>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>

namespace ro = core::operations::request;
using namespace indicators;

namespace domain::images {
pull_command::pull_command()
  : command("pull"), target(""), session(nullptr), progress_bar(std::make_unique<ProgressBar>(option::BarWidth{ 50 },
                                                     option::Start{ "[" },
                                                     option::Fill{ "■" },
                                                     option::Lead{ "■" },
                                                     option::Remainder{ "-" },
                                                     option::End{ " ]" },
                                                     option::PostfixText{ "" },
                                                     option::ForegroundColor{ Color::cyan },
                                                     option::FontStyles{ std::vector<FontStyle>{ FontStyle::bold } }))
{}
void pull_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::arg(target, "target")
    .required()
    .help("specify the oci image to fetch [registry]/[repository]:[tag]"));
}
void pull_command::on_invockation(const lyra::group &g)
{
  // Hide cursor
  show_console_cursor(false);
  session = std::make_unique<staged_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
// callback listeners
void pull_command::on_start() 
{
    pull_order order{};
    order.credentials = "";
    order.target = target;
    auto content = pack_pull_order(order);
    session->write(ro::operation::pull, ro::target::image, content);
}
void pull_command::on_progress(const std::string &feed, float progress)
{
  if (progress > 0 && progress <= 100) {
    progress_bar->set_progress(progress);// all done
  }
  progress_bar->set_option(option::PostfixText{ feed });
}
void pull_command::on_finish(bool is_failure, const std::string &message)
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
pull_command::~pull_command() {}
}// namespace domain::images