#include <core/sessions/interactive_session.h>
#include <domain/images/build_command.h>
#include <domain/images/parser.h>
#include <fmt/color.h>
#include <filesystem>

namespace fs = std::filesystem;
namespace ro = core::operations::request;

namespace domain::images {
build_command::build_command(): 
command("build"), 
file_path("."), 
name(""), 
session(nullptr)
{}
void build_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(file_path, "file")
                     .name("-f")
                     .name("--file")
                     .required()
                     .help("name of the description file (default: `PATH/${NAME}file`)"));
  cmd.add_argument(lyra::opt(name, "tag")
                    .name("-t")
                    .name("--tag")
                    .required()
                    .help("name and optionally a tag (format: 'name:tag')"));
}
void build_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<interactive_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void build_command::on_start(bool is_remote)
{
  fs::path description_file_path(file_path);
  std::error_code error;
  if (auto path_exists = fs::exists(description_file_path, error); error || !path_exists) {
    session->disconnect();
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "build error: {}!\n", error.message());
  } else if (auto result = parse_description(description_file_path); !result) {
    session->disconnect();
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "build error: failed to parse description file!\n");
  } else {
    build_order order = create_build_order(result.value(), name, description_file_path.parent_path());
    auto content = pack_build_order(order);
    session->write(ro::operation::build, ro::target::image, content);
  }
}

void build_command::on_data_received(const std::vector<uint8_t> &data) {
    std::string entry(data.begin(), data.end());
    fmt::print("{}", entry);
}

void build_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  if(is_failure)
  {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  } else 
  {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "build success: {}!\n", message);
  }
  fmt::print(fg(fmt::color::white), "\n");
}
build_command::~build_command() {}
}// namespace domain::images