#include "parser.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

auto Parser::db_validator_cb_factory() -> json::parser_callback_t {
  return [is_port_key = false](int depth, json::parse_event_t event,
                               json& parsed_element) mutable {
    if (event == json::parse_event_t::key) {
      is_port_key = (parsed_element == "port");
      return true;
    }

    if (event == json::parse_event_t::value && is_port_key && depth == 2) {
      is_port_key = false;
      if (parsed_element.is_number_integer()) {
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

auto Parser::load(const std::filesystem::path& filepath) -> json {
  if (!std::filesystem::exists(filepath)) {
    throw std::filesystem::filesystem_error(
        "File or directory does not exist: ", filepath,
        std::make_error_code(std::errc::no_such_file_or_directory));
  }

  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open: " + filepath.string());
  }

  return json::parse(file, Parser::db_validator_cb_factory(), true, false);
}
