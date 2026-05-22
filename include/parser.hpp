#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

using nlohmann::json;

class Parser {
 public:
  static auto load(const std::filesystem::path& filepath) -> json;

 private:
  static auto db_validator_cb_factory() -> json::parser_callback_t;
};
