#include <fmt/base.h>
#include <fmt/ostream.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#include "printer.hpp"

// Type aliases
using Params = DatabaseConfig::ConnectionParams;
using Catch::Matchers::ContainsSubstring;
using Catch::Matchers::Message;

TEST_CASE("Printer accurately displays database configurations", "[printer]") {
  Printer printer_obj;
  std::stringstream ss;

  SECTION("Displays all fields to standard output using the default argument") {
    DatabaseConfig config(Params{.username = "admin",
                                 .password = "secret123",
                                 .host = "192.168.1.5",
                                 .port = 5432,
                                 .name = "production_db",
                                 .dbTypeStr = "PostgresQL"});

    printer_obj.printer(config, ss);
    std::string output = ss.str();

    int width = printer_obj.getLabelWidth();

    CHECK_THAT(output, ContainsSubstring(fmt::format(
                           "{:<{}} {}", "Username:", width, "admin")));
    CHECK_THAT(output, ContainsSubstring(fmt::format(
                           "{:<{}} {}", "Password:", width, "secret123")));
    CHECK_THAT(output, ContainsSubstring(fmt::format(
                           "{:<{}} {}", "Host:", width, "192.168.1.5")));
    CHECK_THAT(output, ContainsSubstring(
                           fmt::format("{:<{}} {}", "Port:", width, 5432)));
    CHECK_THAT(output,
               ContainsSubstring(fmt::format(
                   "{:<{}} {}", "Database Name:", width, "production_db")));
  }

  SECTION("Completely hides missing optional fields from the targeted stream") {
    DatabaseConfig config(Params{.username = std::nullopt,
                                 .password = std::nullopt,
                                 .host = std::nullopt,
                                 .port = std::nullopt,
                                 .name = "test_db",
                                 .dbTypeStr = "SQLite"});

    printer_obj.printer(config, ss);
    std::string output = ss.str();
    int width = printer_obj.getLabelWidth();

    CHECK_THAT(output, ContainsSubstring(fmt::format(
                           "{:<{}} {}", "Database Name:", width, "test_db")));
    CHECK_THAT(output, ContainsSubstring(fmt::format(
                           "{:<{}} {}", "Database Type:", width, 'S')));

    CHECK_THAT(output, !ContainsSubstring("Username:"));
    CHECK_THAT(output, !ContainsSubstring("Password:"));
    CHECK_THAT(output, !ContainsSubstring("Host:"));
    CHECK_THAT(output, !ContainsSubstring("Port:"));
  }
}

TEST_CASE("Printer validation and dynamic padding controls", "[printer]") {
  std::string expected_msg =
      fmt::format("Invalid label width specified for Printer (width >= {}).",
                  MIN_LABEL_WIDTH);
  SECTION(
      "Accepts valid custom widths above or equal to the minimum constant") {
    CHECK_NOTHROW(Printer(MIN_LABEL_WIDTH));

    Printer variable_printer(25);
    CHECK(variable_printer.getLabelWidth() == 25);
  }

  SECTION(
      "Throws std::invalid_argument when constructor width is below minimum") {
    CHECK_THROWS_MATCHES(Printer(MIN_LABEL_WIDTH - 1), std::invalid_argument,
                         Message(expected_msg));
  }

  SECTION(
      "Throws std::invalid_argument when setLabelWidth receives a bad value") {
    Printer default_printer;

    REQUIRE_THROWS_MATCHES(default_printer.setLabelWidth(MIN_LABEL_WIDTH - 1),
                           std::invalid_argument, Message(expected_msg));
    CHECK(default_printer.getLabelWidth() == DEFAULT_LABEL_WIDTH);
  }
}
