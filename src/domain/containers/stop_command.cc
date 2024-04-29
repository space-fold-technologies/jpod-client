#include <core/sessions/rpc_session.h>
#include <domain/containers/payloads.h>
#include <domain/containers/stop_command.h>
#include <fmt/color.h>

namespace ro = core::operations::request;

namespace domain::containers {
stop_command::stop_command() : command("stop"), name(""), session(nullptr) {}
void stop_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::opt(name, "name").name("-n").name("--name").required().help("unique name for container {or identifier}"));
}
void stop_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void stop_command::on_start()
{
  container_term_order order{ name };
  auto content = pack_container_term_order(order);
  session->write(ro::operation::stop, ro::target::container, content);
}
void stop_command::on_response(const std::vector<uint8_t> &data)
{
  session->disconnect();
  auto message = std::string(data.begin(), data.end());
  fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", message);
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
void stop_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  }
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
stop_command::~stop_command() {}
}// namespace domain::containers