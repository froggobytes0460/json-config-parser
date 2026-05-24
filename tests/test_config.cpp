#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <optional>
#include <stdexcept>
#include <string>

#include "config.hpp"

// Type aliases
using Catch::Matchers::ContainsSubstring;
using Params = DatabaseConfig::ConnectionParams;
using DBType = DatabaseConfig::DatabaseType;

SCENARIO("DatabaseConfig initialization and property mapping", "[config]") {
  GIVEN("A valid SQLite configuration") {
    Params params = {.name = std::move("memory_db"),
                     .dbTypeStr = std::move("SQLite")};

    WHEN("instantiated with completely empty credentials") {
      std::optional<DatabaseConfig> config;
      REQUIRE_NOTHROW(config.emplace(params));

      THEN("properties match and credentials remain empty") {
        CHECK(config->getName() == "memory_db");
        CHECK(config->getDbType() == DBType::S);
        CHECK_FALSE(config->getUserName());
        CHECK_FALSE(config->getPassword());
        CHECK_FALSE(config->getHost());
        CHECK_FALSE(config->getPort());
      }
    }
  }

  GIVEN("Valid configurations for network-based database systems") {
    auto [db_string, expected_enum] = GENERATE(table<std::string, DBType>(
        {{"PostgresQL", DBType::P}, {"MySQL", DBType::M}}));

    DYNAMIC_SECTION("Validating setup for: " << db_string) {
      Params params{.username = "user",
                    .password = "pass",
                    .host = "localhost",
                    .port = 5432,
                    .name = "prod_db",
                    .dbTypeStr = db_string};

      std::optional<DatabaseConfig> config;
      REQUIRE_NOTHROW(config.emplace(params));

      CHECK(config->getDbType() == expected_enum);
      CHECK(config->getName() == "prod_db");
      CHECK(config->getUserName() == "user");
      CHECK(config->getPassword() == "pass");
      CHECK(config->getHost() == "localhost");
      CHECK(config->getPort() == 5432);
    }
  }
}

SCENARIO("DatabaseConfig validation failures", "[config]") {
  GIVEN("An invalid parameter configuration") {
    auto [params, expected_error] = GENERATE(table<Params, std::string>(
        {// Unknown dbType: Oracle
         {Params{.name = "test", .dbTypeStr = "Oracle"},
          "Unknown database type: Oracle"},

         // Name not provided
         {Params{.name = "", .dbTypeStr = "SQLite"},
          "Database configuration requires name."},

         // Port not provided for network-based database system
         {Params{.username = "user",
                 .password = "pass",
                 .host = "localhost",
                 .name = "prod_db",
                 .dbTypeStr = "PostgresQL"},
          "PostgresQL requires username, password, host, and port."},

         // Illegal network-based database system configuration for SQLITE
         // configuration
         {Params{
              .username = "admin", .name = "memory_db", .dbTypeStr = "SQLite"},
          "SQLite configuration cannot contain username, password, host, or "
          "port."}}));

    THEN("initialization throws std::invalid_argument") {
      REQUIRE_THROWS_WITH(DatabaseConfig(params),
                          ContainsSubstring(expected_error));
    }
  }
}
