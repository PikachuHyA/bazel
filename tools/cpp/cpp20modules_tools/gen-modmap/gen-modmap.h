#pragma once
#include <iostream>
#include <unordered_set>

#include "common/common.h"

struct ModmapItem {
  std::string name;
  std::string path;
  bool operator==(const ModmapItem &other) const {
    return name == other.name && path == other.path;
  }
  friend std::ostream &operator<<(std::ostream &os, const ModmapItem &item) {
    os << "ModmapItem{name: " << item.name << ", path: " << item.path << "}";
    return os;
  }
};
// Define the hash function for the ModmapItem
namespace std {
template <> struct hash<ModmapItem> {
  size_t operator()(const ModmapItem &item) const {
    return hash<string>()(item.name) ^ (hash<string>()(item.path) << 1);
  }
};
} // namespace std
std::unordered_set<ModmapItem> process(const ModuleDep &dep,
                                       const Cpp20ModulesInfo &info,
                                       const std::string &compiler);
void write_modmap(std::ostream &modmap_file_stream,
                  std::ostream &modmap_file_dot_input_stream,
                  const std::unordered_set<ModmapItem> &modmap,
                  const std::string &compiler);