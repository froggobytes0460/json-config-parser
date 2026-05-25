#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class Parser {
 public:
  [[nodiscard]] static auto load(const fs::path& filepath) -> json;

 private:
  static auto db_validator_cb_factory() -> json::parser_callback_t;
};
