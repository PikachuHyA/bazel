#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "agg-ddi.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

// split a string by a delimiter ';' and return a vector of substrings
std::vector<std::string> split_string_by_semicolon(const std::string &str) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string item;

  while (std::getline(ss, item, ';')) {
    if (!item.empty()) {
      result.push_back(item);
    }
  }
  return result;
}

void write_output(std::ostream &output, const Cpp20ModulesInfo &info) {
  json data;
  data["modules"] = info.modules;
  data["usages"] = info.usages;
  output << data.dump(4);
}
