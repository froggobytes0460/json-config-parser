#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

using nlohmann::json;

class Parser {
  json load(const std::filesystem::path& filepath);
};
