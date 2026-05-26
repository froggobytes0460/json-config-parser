#include <fmt/base.h>
#include <fmt/format.h>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "config.hpp"
#include "deserializer.hpp"
#include "parser.hpp"
#include "printer.hpp"

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
  try {
    // JSON file parsing
    json data = Parser::load(json_file);

    // Deserialize from json object
    DatabaseConfig cfg = Deserializer::from_json(data);

    // Print configuration object
    Printer printer_obj =
        labelWidth.has_value() ? Printer(labelWidth.value()) : Printer();
    printer_obj.printer(cfg, out_stream);
  } catch (json::parse_error& err) {
    fmt::print(stderr, "Broken/Invalid JSON file provided.\n\n{}",
               program.help().str());
  } catch (json::out_of_range& err) {
    fmt::print(stderr, "Broken/Invalid JSON file provided.\n\n{}",
               program.help().str());
  } catch (std::invalid_argument& err) {
    fmt::print(stderr, "{}\n\n{}", err.what(), program.help().str());
  } catch (std::exception& err) {
    fmt::print(stderr, "{}\n\n{}", err.what(), program.help().str());
  }

  return 0;
}
