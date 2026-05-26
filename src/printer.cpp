#include "printer.hpp"

#include <fmt/base.h>
#include <fmt/ostream.h>
#include <fmt/std.h>

#include <stdexcept>

void Printer::printer(const DatabaseConfig& config, std::ostream& out) {
  if (config.getUserName().has_value()) {
    fmt::print(out, "{:<{}} {}\n", "Username:", labelWidth,
               config.getUserName().value());
  }

  if (config.getPassword().has_value()) {
    fmt::print(out, "{:<{}} {}\n", "Password:", labelWidth,
               config.getPassword().value());
  }
  if (config.getHost().has_value()) {
    fmt::print(out, "{:<{}} {}\n", "Host:", labelWidth,
               config.getHost().value());
  }

  if (config.getPort().has_value()) {
    fmt::print(out, "{:<{}} {}\n", "Port:", labelWidth,
               config.getPort().value());
  }

  fmt::print(out, "{:<{}} {}\n", "Database Name:", labelWidth,
             config.getName());

  fmt::print(out, "{:<{}} {}\n", "Database Type:", labelWidth,
             static_cast<char>(config.getDbType()));
}

void Printer::setLabelWidth(int width) {
  if (width < MIN_LABEL_WIDTH) {
    throw std::invalid_argument(
        fmt::format("Invalid label width specified for Printer (width >= {}).",
                    MIN_LABEL_WIDTH));
  }
  labelWidth = width;
}
