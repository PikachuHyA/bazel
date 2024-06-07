#include "gen-modmap.h"
#include <gtest/gtest.h>

TEST(ModmapTest, EmptyInput) {
  std::string ddi_content = R"(
  {
    "revision": 0,
    "rules": [
      {
        "primary-output": "demo.o"
      }
    ],
    "version": 1
  }

  )";

  std::string info_content = R"(
  {"modules":{},"usages":{}}
  )";

  std::istringstream ddi_stream(ddi_content);
  std::istringstream info_stream(info_content);

  auto modmap = process(ddi_stream, info_stream, "clang");

  std::unordered_set<ModmapItem> expected_modmap;
  EXPECT_EQ(modmap, expected_modmap);
}

TEST(ModmapTest, BasicFunctionality) {
  std::string ddi_content = R"({
        "rules": [{
            "requires": [{
                "logical-name": "module1"
            }, {
                "logical-name": "module2"
            }]
        }]
    })";

  std::string info_content = R"({
        "modules": {
            "module1": "/path/to/module1",
            "module2": "/path/to/module2"
        },
        "usages": {
            "module1": ["module2"],
            "module2": []
        }
    })";

  std::istringstream ddi_stream(ddi_content);
  std::istringstream info_stream(info_content);

  auto modmap = process(ddi_stream, info_stream);

  std::unordered_set<ModmapItem> expected_modmap = {{"module1", "/path/to/module1"},
                                             {"module2", "/path/to/module2"}};
  EXPECT_EQ(modmap, expected_modmap);
}
