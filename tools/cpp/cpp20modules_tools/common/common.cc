#include "common.h"

#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

ModuleDep parse_ddi(std::istream &ddi_stream) {
  ModuleDep dep{};
  json data = json::parse(ddi_stream);
  auto &rules = data["rules"];
  if (rules.size() > 1) {
    std::cerr << "bad ddi" << std::endl;
    std::exit(1);
  }
  if (rules.empty()) {
    return dep;
  }
  auto &provides = rules[0]["provides"];
  if (provides.size() > 1) {
    std::cerr << "bad ddi" << std::endl;
    std::exit(1);
  }
  if (provides.size() == 1) {
    std::string name = provides[0]["logical-name"];
    dep.gen_bmi = true;
    dep.name = name;
  }
  for (const auto &item : rules[0]["requires"]) {
    dep.require_list.push_back(item["logical-name"]);
  }
  return dep;
}
Cpp20ModulesInfo parse_info(std::istream &info_stream) {
  json data = json::parse(info_stream);
  Cpp20ModulesInfo info;
  for (const auto &item : data["modules"].items()) {
    info.modules[item.key()] = item.value();
  }
  for (const auto &item : data["usages"].items()) {
    info.usages[item.key()] = item.value();
  }
  return info;
}
