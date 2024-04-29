#include <core/sessions/rpc_session.h>
#include <domain/containers/create_command.h>
#include <domain/containers/payloads.h>
#include <fmt/color.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

namespace ro = core::operations::request;
using namespace ranges;

namespace domain::containers {
create_command::create_command()
  : command("create"), name(""), image(""), env_vars(), port_maps(), network("default"), session(nullptr)
{}
void create_command::on_setup(lyra::command &cmd)
{
  cmd.add_argument(lyra::opt(name, "name").name("-n").name("--name").required().help("unique name for container"));
  cmd.add_argument(lyra::opt(image, "image")["-i"]["--image"].required()("target image to base container on"));
  cmd.add_argument(lyra::opt(env_vars, "env-vars")["-e"]["--env"].cardinality(0, 0)("add environment variables"));

  cmd.add_argument(
    lyra::opt(port_maps, "publish")["-p"]["--publish"].cardinality(0, 0)("publish a container's port to host"));
  cmd.add_argument(lyra::opt(network, "network")["--network"].optional()("connect a container to a network"));
}
void create_command::on_invockation(const lyra::group &g)
{
  session = std::make_unique<rpc_session>(*this);
  // now to set up the needed operational resources
  session->connect();
}
void create_command::on_start()
{
  container_creation_order order;
  order.tagged_image = image;
  order.name = name;
  order.network_properties = network;
  auto env_pair = [](const std::string &entry) -> std::pair<std::string, std::string> {
    auto parts = views::split(entry, '=') | to<std::vector<std::string>>();
    return std::make_pair(parts.at(0), parts.at(1));
  };
  auto port_map_pair = [](const std::string &entry) -> std::pair<std::string, std::string> {
    auto parts = views::split(entry, ':') | to<std::vector<std::string>>();
    return std::make_pair(parts.at(0), parts.at(1));
  };
  if (env_vars.size() > 0) {
    order.env_vars = env_vars | views::transform(env_pair) | to<std::map<std::string, std::string>>();
  }
  if (port_maps.size() > 0) {
    order.port_map = port_maps | views::transform(port_map_pair) | to<std::map<std::string, std::string>>();
  }
  auto content = pack_container_creation_order(order);
  session->write(ro::operation::build, ro::target::container, content);
}
void create_command::on_response(const std::vector<uint8_t> &data)
{
  std::string payload(data.begin(), data.end());
  if (!payload.empty() && payload.size() == 36) {
    container_term_order order{ payload };
    auto content = pack_container_term_order(order);
    session->write(ro::operation::start, ro::target::container, content);
  } else {
    session->disconnect();
    auto message = std::string(data.begin(), data.end());
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "✔ {}!\n", message);
    fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
  }
}
void create_command::on_finish(bool is_failure, const std::string &message)
{
  session->disconnect();
  if (is_failure) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "✘:{}!", message);
  } else {
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "\n✔{}", message);
  }
  fmt::print(fg(fmt::color::white) | fmt::emphasis::bold, "\n");
}
create_command::~create_command() {}
}// namespace domain::containers