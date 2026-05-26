#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "parser.hpp"

// Type aliases
const fs::path FIXTURE_DIR{"fixtures/"};
using Catch::Matchers::Message;

TEST_CASE("Parser::load properly loads from JSON files.", "[parser]") {
  SECTION("Successful File Reading & Parsing") {
    SECTION("Loads a valid network configuration") {
      json data = Parser::load(FIXTURE_DIR / "normal_network.json");

      CHECK(data["username"] == "db_user");
      CHECK(data["password"] == "secure_pass_123");
      CHECK(data["host"] == "127.0.0.1");
      CHECK(data["port"] == 5432);
      CHECK(data["name"] == "metrics_warehouse");
      CHECK(data["db_type"] == "PostgresQL");
    }

    SECTION("Loads a valid local configuration") {
      json data = Parser::load(FIXTURE_DIR / "normal_sqlite.json");

      CHECK(data["name"] == "cache_store");
      CHECK(data["dbTypeStr"] == "SQLite");
    }
  }

  SECTION("Error cases.") {
    SECTION("Broken JSONs.") {
      REQUIRE_THROWS_AS(Parser::load(FIXTURE_DIR / "broken.json"),
                        json::parse_error);
    }

    SECTION("Invalid JSONs.") {
      SECTION("Network Configuration.") {
        REQUIRE_THROWS_MATCHES(
            Parser::load(FIXTURE_DIR / "invalid_network.json"),
            std::out_of_range,
            Message("Configuration Error: Port number out of valid range "
                    "(0-65535)."));
      }

      SECTION("Local Configuration.") {
        json data = Parser::load(FIXTURE_DIR / "invalid_sqlite.json");

        CHECK_FALSE(data.contains("name"));
        CHECK(data["dbTypeStr"] == "SQLite");
      }
    }

    SECTION("Non-existent files.") {
      REQUIRE_THROWS_AS(
          Parser::load(FIXTURE_DIR / "this_file_does_not_exist.json"),
          fs::filesystem_error);
    }
  }
}
