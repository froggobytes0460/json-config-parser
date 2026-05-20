#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <optional>
#include <stdexcept>
#include <string>

#include "config.hpp"

using Catch::Matchers::ContainsSubstring;

SCENARIO("DatabaseConfig validation and state tracking", "[config]") {
  GIVEN("A valid SQLite configuration structure") {
    auto name = "memory_db";
    auto type = "SQLite";

    WHEN("instantiated with completely empty credentials") {
      std::optional<DatabaseConfig> config;

      REQUIRE_NOTHROW(config.emplace(
          DatabaseConfig::ConnectionParams{.username = std::nullopt,
                                           .password = std::nullopt,
                                           .host = std::nullopt,
                                           .port = std::nullopt,
                                           .name = name,
                                           .dbTypeStr = type}));

      THEN("the state properties are accurately assigned") {
        REQUIRE(config->getName() == name);
        REQUIRE(config->getDbType() == DatabaseConfig::DatabaseType::S);
        REQUIRE_FALSE(config->getUserName().has_value());
        REQUIRE_FALSE(config->getPassword().has_value());
        REQUIRE_FALSE(config->getHost().has_value());
        REQUIRE_FALSE(config->getPort().has_value());
      }
    }
  }

  GIVEN("Valid configurations for network-based database systems") {
    auto [db_string, expected_enum] =
        GENERATE(table<std::string, DatabaseConfig::DatabaseType>(
            {{"PostgresQL", DatabaseConfig::DatabaseType::P},
             {"MySQL", DatabaseConfig::DatabaseType::M}}));

    DYNAMIC_SECTION("Validating setup for: " << db_string) {
      std::optional<DatabaseConfig> config;

      REQUIRE_NOTHROW(config.emplace(
          DatabaseConfig::ConnectionParams{.username = "user",
                                           .password = "pass",
                                           .host = "localhost",
                                           .port = 5432,
                                           .name = "prod_db",
                                           .dbTypeStr = db_string}));

      REQUIRE(config->getDbType() == expected_enum);
      REQUIRE(config->getName() == "prod_db");
      REQUIRE(config->getUserName() == "user");
      REQUIRE(config->getPassword() == "pass");
      REQUIRE(config->getHost() == "localhost");
      REQUIRE(config->getPort() == 5432);
    }
  }
}

SCENARIO("DatabaseConfig rejects invalid structures", "[config]") {
  GIVEN("An invalid parameter configuration") {
    auto [username, password, host, port, db_name, db_type, expected_error] =
        GENERATE(table<std::optional<std::string>, std::optional<std::string>,
                       std::optional<std::string>, std::optional<int>,
                       std::string, std::string, std::string>({

            // 1. Unknown database type
            {std::nullopt, std::nullopt, std::nullopt, std::nullopt, "test",
             "Oracle", "Unknown database type: Oracle"},

            // 2. Empty target database name
            {std::nullopt, std::nullopt, std::nullopt, std::nullopt, "",
             "SQLite", "Database configuration requires name."},

            // 3. Network DB missing parameters (e.g., missing port)
            {"user", "pass", "localhost", std::nullopt, "prod_db", "PostgresQL",
             "PostgresQL requires username, password, host, and port."},

            // 4. Invalid port value
            {"user", "pass", "localhost", -3, "prod_db", "PostgresQL",
             "Port must be a positive integer."},

            // 5. SQLite holding restricted credentials
            {"admin", std::nullopt, std::nullopt, std::nullopt, "memory_db",
             "SQLite",
             "SQLite configuration cannot contain username, password, host, or "
             "port."}}));

    THEN(
        "initialization throws std::invalid_argument with the correct "
        "explanation") {
      REQUIRE_THROWS_WITH(DatabaseConfig(DatabaseConfig::ConnectionParams{
                              .username = username,
                              .password = password,
                              .host = host,
                              .port = port,
                              .name = db_name,
                              .dbTypeStr = db_type}),
                          ContainsSubstring(expected_error));
    }
  }
}
