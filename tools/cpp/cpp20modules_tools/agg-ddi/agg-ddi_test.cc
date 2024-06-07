#include "agg-ddi.h"
#include <gtest/gtest.h>

TEST(SplitStringBySemicolonTest, BasicFunctionality) {
  std::string input = "a;b;c";
  std::vector<std::string> expected = {"a", "b", "c"};
  EXPECT_EQ(split_string_by_semicolon(input), expected);
}

TEST(Cpp20ModulesInfoTest, BasicFunctionality) {
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

  std::istringstream info_stream(info_content);
  bool verbose = false;
  Cpp20ModulesInfo full_info{};
  process_cpp20modules_info(full_info, info_stream, verbose);

  EXPECT_EQ(full_info.modules["module1"], "/path/to/module1");
  EXPECT_EQ(full_info.modules["module2"], "/path/to/module2");
  EXPECT_EQ(full_info.usages["module1"].size(), 1);
  EXPECT_EQ(full_info.usages["module1"][0], "module2");
  EXPECT_EQ(full_info.usages["module2"].size(), 0);
}

TEST(Cpp20ModulesInfoTest, BasicFunctionalityWithTwoFile) {
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

  std::string info_content2 = R"({
        "modules": {
            "foo": "/path/to/foo",
            "bar": "/path/to/bar"
        },
        "usages": {
            "foo": [],
            "bar": ["foo"]
        }
    })";

  std::istringstream info_stream(info_content);
  std::istringstream info_stream2(info_content2);
  bool verbose = false;
  Cpp20ModulesInfo full_info{};
  process_cpp20modules_info(full_info, info_stream, verbose);
  process_cpp20modules_info(full_info, info_stream2, verbose);

  EXPECT_EQ(full_info.modules["module1"], "/path/to/module1");
  EXPECT_EQ(full_info.modules["module2"], "/path/to/module2");
  EXPECT_EQ(full_info.modules["foo"], "/path/to/foo");
  EXPECT_EQ(full_info.modules["bar"], "/path/to/bar");
  EXPECT_EQ(full_info.usages["module1"].size(), 1);
  EXPECT_EQ(full_info.usages["module1"][0], "module2");
  EXPECT_EQ(full_info.usages["module2"].size(), 0);
  EXPECT_EQ(full_info.usages["bar"].size(), 1);
  EXPECT_EQ(full_info.usages["bar"][0], "foo");
  EXPECT_EQ(full_info.usages["foo"].size(), 0);
}

TEST(WriteOutputTest, BasicFunctionality) {
  Cpp20ModulesInfo info;
  info.modules["module1"] = "/path/to/module1";
  info.modules["module2"] = "/path/to/module2";
  info.usages["module1"].push_back("module2");

  std::ostringstream output_stream;
  write_output(output_stream, info);

  std::string expected_output = R"({
    "modules": {
        "module1": "/path/to/module1",
        "module2": "/path/to/module2"
    },
    "usages": {
        "module1": [
            "module2"
        ]
    }
})";
  EXPECT_EQ(output_stream.str(), expected_output);
}

TEST(ProcessDdiTest, BasicFunctionality) {
  Cpp20ModulesInfo full_info;
  std::string ddi_content = R"({
        "rules": [{
            "provides": [{
                "logical-name": "module3"
            }],
            "requires": [{
                "logical-name": "module1"
            }]
        }]
    })";

  std::istringstream ddi_stream(ddi_content);
  process_ddi(full_info, ddi_stream, "/path/to/module3", false);

  EXPECT_EQ(full_info.modules["module3"], "/path/to/module3");
  EXPECT_EQ(full_info.usages["module3"].size(), 1);
  EXPECT_EQ(full_info.usages["module3"][0], "module1");
}
