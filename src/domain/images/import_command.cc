#include <core/sessions/rpc_session.h>
#include <domain/images/import_command.h>
#include <domain/images/payloads.h>
#include <filesystem>
#include <fmt/color.h>

namespace fs = std::filesystem;
namespace ro = core::operations::request;

namespace domain::images {
import_command::import_command()
  : command("import"), file_path("."), session(nullptr)
{}
void import_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(file_path, "archive").name("--archive-path").required().help("Image Archive to import."));
}
void import_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
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
  }
}
void import_command::on_response(const std::vector<uint8_t> &data)
{
  
}
void import_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "import error: {}!\n", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "import success: {}!\n", message);
  }
  fmt::print(fg(fmt::color::white), "\n");
}
import_command::~import_command() {}
}// namespace domain::images