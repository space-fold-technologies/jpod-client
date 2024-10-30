#include <core/sessions/interactive_session.h>
#include <domain/containers/payloads.h>
#include <domain/containers/shell_command.h>
#include <domain/containers/terminal.h>
#include <fmt/color.h>

namespace ro = core::operations::request;

namespace domain::containers {
shell_command::shell_command() : command("exec"), name(""), interactive(false), user(""), commands{}, session(nullptr)
{}
void shell_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(
    lyra::opt(name, "name").name("-n").name("--name").required().help("unique name for container {or identifier}"));
  cmd.add_argument(lyra::opt(name, "interactive")
                     .name("-i")
                     .name("--interactive")
                     .optional()
                     .help("keep STDIN open even if not attached"));
  cmd.add_argument(lyra::opt(user, "user")
                     .name("-u")
                     .name("--user")
                     .optional()
                     .help("username or UID (format: '<name|uid>[:<group|gid>]')"));
  cmd.add_argument(lyra::arg(commands, "command").optional().help("The command, and arguments, to attempt to run."));
}
void shell_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<interactive_session>(*this);
  forwarded_terminal = std::make_unique<terminal>(session->context(), *this);
  // now to set up the needed operational resources
  session->connect();
}
void shell_command::on_start(bool is_remote)
{
  if (!is_remote) {
    forwarded_terminal->initiate();
  } else {
    forwarded_terminal->start_io();
  }
}
void shell_command::on_data_received(const std::vector<uint8_t> &data)
{
  if (forwarded_terminal) { forwarded_terminal->write(data); }
}
void shell_command::on_finish(bool is_failure, const std::string &message)
{
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!\n\r", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔{}\n\r", message);
  }
}

void shell_command::on_input_received(const std::vector<uint8_t> &content)
{
  container_shell_order order{ shell_order_type::terminal_feed, content };
  auto payload = pack_container_shell_order(order);
  session->write(ro::operation::shell, ro::target::container, payload);
}
void shell_command::on_terminal_initialized(uint32_t rows, uint32_t columns)
{
  order_properties properties{};
  properties.name = name;
  properties.columns = columns;
  properties.rows = rows;
  properties.commands = commands;
  properties.interactive = interactive;
  properties.user = user;
  container_shell_order order{ shell_order_type::start_session, pack_container_properties(properties) };
  auto content = pack_container_shell_order(order);
  session->write(ro::operation::shell, ro::target::container, content);
}
void shell_command::on_terminal_resized(uint32_t rows, uint32_t columns)
{
  std::string payload = fmt::format("{}:{}", columns, rows);
  std::vector<uint8_t> data(payload.begin(), payload.end());
  container_shell_order order{ shell_order_type::terminal_size, data };
  auto content = pack_container_shell_order(order);
  session->write(ro::operation::shell, ro::target::container, content);
}
void shell_command::on_terminal_error(const std::error_code &err)
{
  if (session) 
  { 
    session->disconnect(); 
  }
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "terminal error:✘ {}!\n", err.message());
}
shell_command::~shell_command() {}
}// namespace domain::containers