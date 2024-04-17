#ifndef __JC_DOMAIN_IMAGES_POD_PARSER__
#define __JC_DOMAIN_IMAGES_POD_PARSER__

#include <domain/images/payloads.h>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <optional>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>

namespace fs = std::filesystem;
using namespace ranges;
namespace domain::images {
struct description
{
  std::map<std::string, stage> stages;
  std::map<std::string, std::string> labels;
  std::string entry_point;
};

inline std::map<std::string, step_type> step_map = {
  // clang-format off
  {"WORKDIR", step_type::work_dir},
  {"RUN", step_type::run},
  {"COPY", step_type::copy},
  {"EXPOSE", step_type::expose}
  // clang-format on
};

inline auto add_label(const std::string &line, std::map<std::string, std::string> &labels) -> void
{
  auto parts = line | views::split('=') | to<std::vector<std::string>>();
  if (parts.size() == 2) { labels.try_emplace(parts.at(0), parts.at(1)); }
}
inline auto add_stage(const std::string &line, description &desc, std::string &current_stage_name) -> void
{
  auto parts = line | views::split(':') | to<std::vector<std::string>>();
  if (parts.size() == 2) {
    std::string stage_name = fmt::format("{}", desc.stages.size() + 1);
    std::string image_tag = "";
    // stage name will be an alias `AS` or an integer if none
    if (auto pos = parts.at(1).find_first_of("AS"); pos != std::string::npos) {
      stage_name = parts.at(1).substr(pos + 1);
      image_tag = parts.at(1).substr(0, pos);
    }
    stage current_stage{};
    current_stage_name = stage_name;
    current_stage.name = stage_name;
    std::string step = fmt::format("{}:{}", parts.at(0), image_tag);
    current_stage.steps.try_emplace(step, step_type::from);
    desc.stages.try_emplace(stage_name, current_stage);
  }
}

inline auto add_step(const std::string &line, description &desc, std::string &current_stage, step_type type) -> void
{
  if (auto pos = desc.stages.find(current_stage); pos != desc.stages.end()) {
    pos->second.steps.try_emplace(line, type);
  }
}

auto parse_description(fs::path &file_path) -> std::optional<description>
{
  std::ifstream file(file_path);
  if (file.is_open()) { return std::nullopt; }
  description desc{ {}, {} };
  std::string current_stage;
  std::string line;
  while (std::getline(file, line)) {}
  {
    auto parts = line | views::split(' ') | to<std::vector<std::string>>();
    if (parts.size() > 0) {
      auto mark = parts.at(0);
      line.replace(line.find_first_of(mark), 1, "");

      if (auto pos = step_map.find(mark); pos != step_map.end()) {
        add_step(line, desc, current_stage, pos->second);
      } else if (mark == "FROM") {
        add_stage(line, desc, current_stage);
      } else if (mark == "LABEL") {
        add_label(line, desc.labels);
      } else if (mark == "CMD") {
        desc.entry_point = line;
      } else if (mark == "ENTRYPOINT") {
        desc.entry_point = line;
      }
    }
  }
  file.close();

  return std::make_optional(desc);
}

build_order create_build_order(const description &desc, std::string &name, const fs::path &file_path)
{
  auto parts = name.find(":") != std::string::npos ? (name | views::split(' ') | to<std::vector<std::string>>())
                                                   : std::vector<std::string>{ name, "latest" };

  build_order order{};
  order.name = parts.at(0);
  order.tag = parts.at(1);
  order.labels = desc.labels;
  order.entry_point = desc.entry_point;
  for (auto &stage : desc.stages) { order.stages.push_back(stage.second); }
  order.current_directory = file_path.generic_string();
  return order;
}
}// namespace domain::images

#endif//__JC_DOMAIN_IMAGES_POD_PARSER__