#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <optional>
#include <stdexcept>

#include "config.hpp"
#include "deserializer.hpp"

// Type aliases
using Catch::Matchers::ContainsSubstring;

struct SuccessTestCase {
  std::string case_name;
  json input_json;
  DatabaseConfig::DatabaseType expected_type;
  std::string expected_name;
};

TEST_CASE(
    "Deserializer::from_json handles valid configurations via generator matrix",
    "[deserializer]") {
  SuccessTestCase const& test_case = GENERATE(values<SuccessTestCase>(
      {{.case_name = "Local configuration with missing keys",
        .input_json = {{"db_type", "SQLite"}, {"name", "local_cache"}},
        .expected_type = DatabaseConfig::DatabaseType::S,
        .expected_name = "local_cache"},
       {.case_name = "Converting explicit empty strings to std::nullopt",
        .input_json = {{"db_type", "SQLite"},
                       {"name", "clean_sqlite_db"},
                       {"username", ""},
                       {"password", ""},
                       {"host", ""}},
        .expected_type = DatabaseConfig::DatabaseType::S,
        .expected_name = "clean_sqlite_db"},
       {.case_name = "Network configuration parsing with full attributes",
        .input_json = {{"db_type", "PostgresQL"},
                       {"name", "production_db"},
                       {"username", "admin"},
                       {"password", "secret123"},
                       {"host", "127.0.0.1"},
                       {"port", 5432}},
        .expected_type = DatabaseConfig::DatabaseType::P,
        .expected_name = "production_db"}}));

  DYNAMIC_SECTION("Executing scenario: " << test_case.case_name) {
    std::optional<DatabaseConfig> config;
    REQUIRE_NOTHROW(
        config.emplace(Deserializer::from_json(test_case.input_json)));

    CHECK(config->getDbType() == test_case.expected_type);
    CHECK(config->getName() == test_case.expected_name);

    if (test_case.expected_type == DatabaseConfig::DatabaseType::S) {
      CHECK_FALSE(config->getUserName().has_value());
      CHECK_FALSE(config->getPassword().has_value());
      CHECK_FALSE(config->getHost().has_value());
      CHECK_FALSE(config->getPort().has_value());
    } else {
      CHECK(config->getUserName() ==
            test_case.input_json.value("username", ""));
      CHECK(config->getPort() == test_case.input_json.value("port", 0));
    }
  }
}

TEST_CASE("Deserializer::from_json validation failures propagate correctly",
          "[deserializer]") {
  SECTION("Missing structural attributes throw json out_of_range") {
    json structural_error_json{{"name", "broken_db"}};
    REQUIRE_THROWS_AS(Deserializer::from_json(structural_error_json),
                      json::out_of_range);
  }

  SECTION("Illegal explicit keys trigger business logic errors") {
    json invalid_sqlite_json = {{"db_type", "SQLite"},
                                {"name", "malformed_sqlite"},
                                {"username", "should_not_exist_here"}};

    REQUIRE_THROWS_WITH(
        Deserializer::from_json(invalid_sqlite_json),
        ContainsSubstring("SQLite configuration cannot contain username, "
                          "password, host, or port."));
  }
}
