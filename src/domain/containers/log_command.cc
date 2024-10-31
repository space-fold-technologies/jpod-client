#include <core/sessions/interactive_session.h>
#include <domain/containers/log_command.h>
#include <domain/containers/payloads.h>
#include <fmt/color.h>

namespace ro = core::operations::request;

namespace domain::containers {
log_command::log_command() : command("logs"), name(""), follow(false), tail(0), timestamps(false), session(nullptr) {}
void log_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::opt(name, "name").name("--name").required().help("unique name for container {or identifier}"));
  cmd.add_argument(lyra::opt(follow, "follow").name("-f").name("--follow").optional().help("follow log output"));
  cmd.add_argument(lyra::opt(tail, "tail")
                     .name("-n")
                     .name("--tail")
                     .optional()
                     .help("number of lines to show from the end of the logs (default : 'all' )"));

  cmd.add_argument(
    lyra::opt(timestamps, "timestamps").name("-t").name("--timestamps").optional().help("show timestamps"));
}
void log_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<interactive_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void log_command::on_start(bool is_remote)
{
  if (!is_remote) {
    container_log_order order{ name, follow, tail, timestamps };
    auto content = pack_container_log_order(order);
    session->write(ro::operation::logs, ro::target::container, content);
  }
}
void log_command::on_data_received(const std::vector<uint8_t> &data) {
    std::string entry(data.begin(), data.end());
    fmt::print(entry);
}
void log_command::on_finish(bool is_failure, const std::string &message)
{
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "\n✔{}", message);
  }
}
log_command::~log_command() {}
}// namespace domain::containers