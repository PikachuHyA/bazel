#include "gen-modmap.h"
#include <fstream>
int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: gen-modmap <ddi-file> <cpp20modules-info-file> "
                 "<output> <compiler>"
              << std::endl;
    std::exit(1);
  }

  // Retrieve the values of the flags
  std::string ddi_filename = argv[1];
  std::string info_filename = argv[2];
  std::string output = argv[3];
  std::string compiler = argv[4];

  std::ifstream info_stream(info_filename);
  if (!info_stream.is_open()) {
    std::cerr << "ERROR: Failed to open the file " << info_filename
              << std::endl;
    std::exit(1);
  }
  std::ifstream ddi_stream(ddi_filename);
  if (!ddi_stream.is_open()) {
    std::cerr << "ERROR: Failed to open the file " << ddi_filename << std::endl;
    std::exit(1);
  }
  auto dep = parse_ddi(ddi_stream);
  auto info = parse_info(info_stream);
  auto modmap = process(dep, info, compiler);

  std::string modmap_filename = output;
  std::string modmap_dot_input_filename = modmap_filename + ".input";
  std::ofstream modmap_file_stream(modmap_filename);
  std::ofstream modmap_file_dot_input_stream(modmap_dot_input_filename);
  if (!modmap_file_stream.is_open()) {
    std::cerr << "ERROR: Failed to open the file " << modmap_filename
              << std::endl;
    std::exit(1);
  }
  if (!modmap_file_dot_input_stream.is_open()) {
    std::cerr << "ERROR: Failed to open the file " << modmap_dot_input_filename
              << std::endl;
    std::exit(1);
  }
  if (compiler == "gcc") {
    modmap_file_stream << "$root ."
                       << "\n";
    if (dep.gen_bmi) {
      modmap_file_stream << dep.name << " " << info.modules[dep.name] << "\n";
    }
  }
  write_modmap(modmap_file_stream, modmap_file_dot_input_stream, modmap,
               compiler);
  modmap_file_stream.close();
  modmap_file_dot_input_stream.close();

  return 0;
}