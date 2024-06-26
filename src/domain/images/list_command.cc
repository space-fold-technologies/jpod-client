#include <core/commands/formatter.h>
#include <core/sessions/rpc_session.h>
#include <domain/images/list_command.h>
#include <domain/images/payloads.h>
#include <fmt/color.h>
#include <range/v3/algorithm/for_each.hpp>
#include <tabulate/table.hpp>

namespace ro = core::operations::request;
using namespace tabulate;

namespace domain::images {
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
  image_term_order order{ query };
  auto content = pack_image_term_order(order);
  session->write(ro::operation::list, ro::target::image, content);
}
void list_command::on_response(const std::vector<uint8_t> &data)
{
  session->disconnect();
  auto summary = unpack_summary(data);
  Table table;
  table.add_row({ "NAME", "REPOSITORY", "TAG", "IMAGE ID", "CREATED", "SIZE" });
  for (std::size_t i = 0; i < 6; ++i) {
    table.format().font_color(Color::yellow).font_align(FontAlign::center).font_style({ FontStyle::bold });
  }
  ranges::for_each(summary.entries, [&table](const auto &entry) {
    table.add_row({ entry.name,
      entry.repository,
      entry.tag,
      entry.identifier,
      core::commands::format_time_ago(entry.created_at),
      core::commands::format_file_size(entry.size) });
  });
  std::cout << table << std::endl;
}
void list_command::on_failure(const std::string &message)
{
  session->disconnect();
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  fmt::print(fg(fmt::color::white), "\n");
}
list_command::~list_command() {}
}// namespace domain::images