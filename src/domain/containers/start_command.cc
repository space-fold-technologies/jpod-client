#include <core/sessions/rpc_session.h>
#include <domain/containers/payloads.h>
#include <domain/containers/start_command.h>
#include <fmt/color.h>

namespace ro = core::operations::request;

namespace domain::containers {
start_command::start_command() : command("start"), name(""), session(nullptr) {}
void start_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::opt(name, "name").name("-n").name("--name").required().help("unique name for container {or identifier}"));
}
void start_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void start_command::on_start()
{

  container_term_order order{ name };
  auto content = pack_container_term_order(order);
  session->write(ro::operation::start, ro::target::container, content);
}
void start_command::on_response(const std::vector<uint8_t> &data)
{
  session->disconnect();
  auto message = std::string(data.begin(), data.end());
  fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", message);
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
void start_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  }
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
start_command::~start_command() {}
}// namespace domain::containers