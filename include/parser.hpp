#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class Parser {
 public:
  [[nodiscard]] static auto load(const fs::path& filepath) -> json;
};
