// Copyright 2024 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <any>
#include <map>
#include <string>
#include <variant>

// Define a basic struct for JSON values
struct JsonValue {
  using ObjectType = std::map<std::string, JsonValue>;
  using ArrayType = std::vector<JsonValue>;

  std::variant<std::string, bool, int, double, ObjectType, ArrayType,
               std::nullptr_t>
      value;

  JsonValue() : value(nullptr) {}
  JsonValue(const std::string &v) : value(v) {}
  JsonValue(const char *v) : value(std::string(v)) {}
  JsonValue(bool v) : value(v) {}
  JsonValue(int v) : value(v) {}
  JsonValue(double v) : value(v) {}
  JsonValue(const ObjectType &v) : value(v) {}
  JsonValue(const ArrayType &v) : value(v) {}
  JsonValue(std::nullptr_t) : value(nullptr) {}

  bool is_null() const { return std::holds_alternative<std::nullptr_t>(value); }
  bool is_string() const { return std::holds_alternative<std::string>(value); }
  bool is_object() const { return std::holds_alternative<ObjectType>(value); }
  bool is_array() const { return std::holds_alternative<ArrayType>(value); }
  bool is_bool() const { return std::holds_alternative<bool>(value); }
  bool is_int() const { return std::holds_alternative<int>(value); }
  bool is_double() const { return std::holds_alternative<double>(value); }

  const std::string &as_string() const { return std::get<std::string>(value); }
  const ObjectType &as_object() const { return std::get<ObjectType>(value); }
  const ArrayType &as_array() const { return std::get<ArrayType>(value); }
  bool as_bool() const { return std::get<bool>(value); }
  int as_int() const { return std::get<int>(value); }
  double as_double() const { return std::get<double>(value); }
};

JsonValue parse_json(const std::string &data);
std::string to_json(const JsonValue &data);
