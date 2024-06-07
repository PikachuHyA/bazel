#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "common/common.h"

std::vector<std::string> split_string_by_semicolon(const std::string &str);
void write_output(std::ostream &output, const Cpp20ModulesInfo &info);