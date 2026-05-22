#include "parser.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

json Parser::load(const std::filesystem::path& filepath) {
  // Preliminary check to ensure filepath is valid.
  if (!std::filesystem::exists(filepath)) {
    throw std::filesystem::filesystem_error(
        "File or directory does not exist: ", filepath,
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  // Check to see if file can be opened.
  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open: " + filepath.string());
  }
  // Database config validator callback
  auto db_validator_cb = [](int depth, json::parse_event_t event,
                            json& parsed_element) {
    if (event == json::parse_event_t::key && parsed_element == "port") {
      return true;
    }

    if (event == json::parse_event_t::value && depth == 2) {
      if (parsed_element.is_number_integer()) {
        int val = parsed_element.get<int>();
        if (val < 0 || val > 65535) {
          return false;
        }
      }
    }
    return true;
  };

  // Return parsed JSON file.
  return json::parse(file, db_validator_cb, true, false);
}
