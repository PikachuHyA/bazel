#include "agg-ddi.h"
#include <fstream>
#include <iostream>
// Main function
int main(int argc, char *argv[]) {
  std::vector<std::string> cpp20modules_info;
  std::vector<std::string> ddi;
  std::vector<std::string> module_file;
  std::string output;
  bool verbose{};
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-m" && i + 1 < argc) {
      cpp20modules_info.emplace_back(argv[++i]);
    } else if (arg == "-d" && i + 2 < argc) {
      ddi.emplace_back(argv[++i]);
      module_file.emplace_back(argv[++i]);
    } else if (arg == "-o" && i + 1 < argc) {
      output = argv[++i];
    } else if (arg == "--verbose") {
      verbose = true;
    } else {
      std::cerr << "ERROR: Unknown or incomplete argument: " << arg
                << std::endl;
      std::exit(1);
    }
  }
  if (output.empty()) {
    std::cerr << "ERROR: output not specified" << std::endl;
    std::exit(1);
  }

  Cpp20ModulesInfo full_info{};

  // Process cpp20modules_info files
  for (const auto &info_filename : cpp20modules_info) {
    std::ifstream info_stream(info_filename);
    auto info = parse_info(info_stream);
    full_info.merge(info);
  }

  // Process ddi files
  for (int i =0;i<ddi.size();i++) {
    auto ddi_filename = ddi[i];
    auto pcm_path = module_file[i];
    std::ifstream ddi_stream(ddi_filename);
    auto dep = parse_ddi(ddi_stream);
    if (dep.gen_bmi) {
      full_info.modules[dep.name] = pcm_path;
      full_info.usages[dep.name] = dep.require_list;
    }
  }

  // Write final output to file
  std::ofstream of(output);
  if (!of.is_open()) {
    std::cerr << "ERROR: Failed to open the file " << output << "\n";
    std::exit(1);
  }
  write_output(of, full_info);

  return 0;
}