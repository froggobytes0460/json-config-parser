#include "deserializer.hpp"

auto Deserializer::from_json(const json& data) -> DatabaseConfig {
  const std::string& db_type_ref =
      data.at("db_type").get_ref<const std::string&>();

  auto extract_optional_string =
      [&](const std::string& key) -> std::optional<std::string> {
    if (!data.contains(key)) return std::nullopt;

    std::string val = data.at(key).get<std::string>();
    if (val.empty()) return std::nullopt;

    return val;
  };

  return DatabaseConfig(DatabaseConfig::ConnectionParams{
      .dbTypeStr = db_type_ref,
      .name = data.at("name").get<std::string>(),
      .username = extract_optional_string("username"),
      .password = extract_optional_string("password"),
      .host = extract_optional_string("host"),
      .port = data.contains("port")
                  ? std::make_optional(data["port"].get<int>())
                  : std::nullopt});
}
