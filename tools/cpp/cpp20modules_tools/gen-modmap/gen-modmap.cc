#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "gen-modmap.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

void write_modmap(std::ostream &modmap_file_stream,
                  std::ostream &modmap_file_dot_input_stream,
                  const std::unordered_set<ModmapItem> &modmap,
                  const std::string &compiler) {
  for (const auto &item : modmap) {
    if (compiler == "clang") {
      modmap_file_stream << "-fmodule-file=" << item.name << "=" << item.path
                         << "\n";
    } else if (compiler == "gcc") {
      modmap_file_stream << item.name << " " << item.path << "\n";
    }
    else if (compiler == "msvc-cl") {
      modmap_file_stream << "/reference " << item.name << "=" << item.path << "\n";
    }
    else {
      std::cerr << "bad compiler: " << compiler << std::endl;
      std::exit(1);
    }
    modmap_file_dot_input_stream << item.path << "\n";
  }
}

std::unordered_set<ModmapItem> process(const ModuleDep &dep,
                                       const Cpp20ModulesInfo &info,
                                       const std::string &compiler) {

  std::queue<std::string> q;
  for (const auto &item : dep.require_list) {
    q.push(item);
  }
  // Get all dependencies
  std::unordered_set<std::string> s;
  while (!q.empty()) {
    std::string name = q.front();
    q.pop();
    s.insert(name);
    auto it = info.usages.find(name);
    if (it == info.usages.end()) {
      continue;
    }
    auto deps = it->second;
    for (const auto &dep : deps) {
      if (s.count(dep)) {
        continue;
      }
      q.push(dep);
    }
  }

  // Construct modmap
  std::unordered_set<ModmapItem> modmap;
  for (const auto &name : s) {
    auto it = info.modules.find(name);
    if (it == info.modules.end()) {
      std::cerr << "ERROR: Module not found: " << name << std::endl;
      std::exit(1);
    }
    modmap.insert(ModmapItem{name, it->second});
  }
  return modmap;
}
