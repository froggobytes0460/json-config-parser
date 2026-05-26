#include <fmt/base.h>
#include <fmt/format.h>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>

auto main(int argc, char** argv) -> int {
  std::filesystem::path json_file;

  // CLI Argument Parsing
  argparse::ArgumentParser program("json_config_parser", "0.1.0");

  program.add_argument("json-file")
      .help("The path to the JSON file to parse.")
      .store_into(json_file);

  program.add_argument("-w", "--label-width")
      .help("Width of the labels, used to customize printing of configuration.")
      .metavar("WIDTH")
      .scan<'i', int>();
  program.add_argument("-l", "--log")
      .help(
          "Redirect layout output from standard output (stdout) to the "
          "standard log stream (clog).")
      .flag();

  try {
    program.parse_args(argc, argv);
    if (!std::filesystem::exists(json_file)) {
      throw std::runtime_error(
          fmt::format("JSON file doesn't exist: '{}'", json_file.string()));
    }
    if (json_file.extension() != ".json") {
      throw std::runtime_error(
          fmt::format("File is not a JSON file: {}", json_file.string()));
    }
  } catch (const std::runtime_error& err) {
    fmt::print(stderr, "{}\n\n{}", err.what(), program.help().str());
    return 1;
  }

  std::optional<int> labelWidth{program.present<int>("--label-width")};
  std::ostream& out_stream = program.get<bool>("--log") ? std::clog : std::cout;

  // JSON file parsing (must be stored in json object)

  // Deserialize from json object

  // Print configuration object
  return 0;
}
