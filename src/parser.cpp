#include "parser.hpp"

#include <fstream>
#include <stdexcept>
#include <system_error>

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

  json data = json::parse(file);

  if (data.contains("port") && !data["port"].is_null()) {
    if (data["port"].is_number()) {
      int val = data["port"].get<int>();
      if (val < 0 || val > 65535) {
        throw std::out_of_range(
            "Configuration Error: Port number out of valid range (0-65535).");
      }
    }
  }

  return data;
}
