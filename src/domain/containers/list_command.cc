#include <core/commands/formatter.h>
#include <core/sessions/rpc_session.h>
#include <domain/containers/list_command.h>
#include <domain/containers/payloads.h>
#include <fmt/color.h>
#include <iostream>
#include <range/v3/algorithm/for_each.hpp>
#include <tabulate/table.hpp>

namespace ro = core::operations::request;
using namespace tabulate;

namespace domain::containers {
list_command::list_command() : command("ls"), query(""), session(nullptr) {}
void list_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(query, "query")
                     .name("-q")
                     .name("--query")
                     .optional()
                     .help("query to filter out the containers you need"));
}
void list_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void list_command::on_start()
{
  container_list_order order{ filter_mode::all, query };
  auto content = pack_container_list_order(order);
  session->write(ro::operation::list, ro::target::container, content);
}
void list_command::on_response(const std::vector<uint8_t> &data)
{
  session->disconnect();
  auto summary = unpack_container_summary(data);

  Table table;
  table.add_row({ "NAME", "IMAGE", "ID", "STATUS", "PORTS", "CREATED" });
  for (std::size_t i = 0; i < 6; ++i) {
    table.format().font_color(Color::yellow).font_align(FontAlign::center).font_style({ FontStyle::bold });
  }
  ranges::for_each(summary.entries, [&table](const auto &entry) {
    table.add_row({ entry.name,
      entry.image,
      entry.identifier,
      entry.status,
      core::commands::format_port_mapping(entry.port_map),
      core::commands::format_time_ago(entry.created_at) });
  });
  
  std::cout << table << std::endl;
}
void list_command::on_finish(bool is_failure, const std::string &message)
{
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "\n✔{}", message);
  }
}
list_command::~list_command() {}
}// namespace domain::containers