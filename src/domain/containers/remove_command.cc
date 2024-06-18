#include <core/sessions/rpc_session.h>
#include <domain/containers/payloads.h>
#include <domain/containers/remove_command.h>
#include <fmt/color.h>

namespace ro = core::operations::request;

namespace domain::containers {
remove_command::remove_command() : command("rm"), name(""), session(nullptr) {}
void remove_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::opt(name, "name").name("-n").name("--name").required().help("unique name for container {or identifier}"));
}
void remove_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void remove_command::on_start()
{

  container_term_order order{ name };
  auto content = pack_container_term_order(order);
  session->write(ro::operation::start, ro::target::container, content);
}
void remove_command::on_response(const std::vector<uint8_t> &data)
{
  session->disconnect();
  auto message = std::string(data.begin(), data.end());
  fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", message);
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
void remove_command::on_failure(const std::string &message)
{
  session->disconnect();
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
remove_command::~remove_command() {}
}// namespace domain::containers