#pragma once
#include <string>
#include <unordered_map>
#include <vector>
struct Cpp20ModulesInfo {
  std::unordered_map<std::string, std::string> modules;
  std::unordered_map<std::string, std::vector<std::string>> usages;

  void merge(const Cpp20ModulesInfo &info) {
    for (const auto &[k, v] : info.modules) {
      modules[k] = v;
    }
    for (const auto &[k, v] : info.usages) {
      usages[k] = v;
    }
  }
};
struct ModuleDep {
  bool gen_bmi;
  std::string name;
  std::vector<std::string> require_list;
};

ModuleDep parse_ddi(std::istream &ddi_stream);
Cpp20ModulesInfo parse_info(std::istream &info_stream);
