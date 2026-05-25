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

TEST_CASE("DatabaseConfig maps valid properties correctly", "[config]") {
  SECTION("SQLite configurations drop credentials") {
    std::optional<DatabaseConfig> config;
    REQUIRE_NOTHROW(
        config.emplace(Params{.name = "memory_db", .dbTypeStr = "SQLite"}));

    CHECK(config->getName() == "memory_db");
    CHECK(config->getDbType() == DBType::S);
    CHECK_FALSE(config->getUserName());
    CHECK_FALSE(config->getPassword());
    CHECK_FALSE(config->getHost());
    CHECK_FALSE(config->getPort());
  }

  auto [db_string, expected_enum] = GENERATE(table<std::string, DBType>(
      {{"PostgresQL", DBType::P}, {"MySQL", DBType::M}}));

  DYNAMIC_SECTION("Network-based setup: " << db_string) {
    std::optional<DatabaseConfig> config;
    REQUIRE_NOTHROW(config.emplace(Params{.username = "user",
                                          .password = "pass",
                                          .host = "localhost",
                                          .port = 5432,
                                          .name = "prod_db",
                                          .dbTypeStr = db_string}));

    CHECK(config->getDbType() == expected_enum);
    CHECK(config->getName() == "prod_db");
    CHECK(config->getUserName() == "user");
    CHECK(config->getPassword() == "pass");
    CHECK(config->getHost() == "localhost");
    CHECK(config->getPort() == 5432);
  }
}

TEST_CASE("DatabaseConfig correctly validates configuration constraints",
          "[config]") {
  auto const& [params, expected_error, case_name] =
      GENERATE(table<Params, std::string, std::string>(
          {{Params{.name = "test", .dbTypeStr = "Oracle"},
            "Unknown database type: Oracle", "Unkown database type error"},

           {Params{.name = "", .dbTypeStr = "SQLite"},
            "Database configuration requires name.",
            "Name not provided for local configuration"},

           {Params{.username = "user",
                   .password = "pass",
                   .host = "localhost",
                   .name = "prod_db",
                   .dbTypeStr = "PostgresQL"},
            "PostgresQL requires username, password, host, and port.",
            "Port not provided for network configuration"},

           {Params{.username = "admin",
                   .name = "memory_db",
                   .dbTypeStr = "SQLite"},
            "SQLite configuration cannot contain username, password, host, or "
            "port.",
            "Illegal network attributes for local configuration"}}));

  DYNAMIC_SECTION("Throws on invalid configurations for: " << case_name) {
    REQUIRE_THROWS_WITH(DatabaseConfig(params),
                        ContainsSubstring(expected_error));
  }
}
