#include <nlohmann/json.hpp>

#include "config.hpp"

using json = nlohmann::json;

class Deserializer {
 public:
  [[nodiscard]] static auto from_json(const json& data) -> DatabaseConfig;
};
