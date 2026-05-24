#include "parser.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

auto Parser::db_validator_cb_factory() -> json::parser_callback_t {
  return [current_key = std::string("")](int depth, json::parse_event_t event,
                                         json& parsed_element) mutable {
    if (event == json::parse_event_t::key) {
      current_key = parsed_element.get<std::string>();
      return true;
    }

    if (event == json::parse_event_t::value && current_key == "port" &&
        depth == 1) {
      current_key = "";

      if (parsed_element.is_number()) {
        int val = parsed_element.get<int>();
        if (val < 0 || val > 65535) {
          throw std::out_of_range(
              "Configuration Error: Port number out of valid range (0-65535).");
        }
      }
    }
    return true;
  };
}

auto Parser::load(const fs::path& filepath) -> json {
  if (!fs::exists(filepath)) {
    throw fs::filesystem_error(
        "File or directory does not exist", filepath,
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open: " + filepath.string());
  }

  return json::parse(file, Parser::db_validator_cb_factory(), true, true);
}
