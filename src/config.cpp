#include "config.hpp"

#include <stdexcept>
#include <utility>

/* Initializer */
DatabaseConfig::DatabaseConfig(ConnectionParams params)
    : username(std::move(params.username)),
      password(std::move(params.password)),
      host(std::move(params.host)),
      port(params.port),
      name(std::move(params.name)),
      dbType([&]() -> DatabaseConfig::DatabaseType {
        if (params.dbTypeStr == "PostgresQL") return DatabaseType::P;
        if (params.dbTypeStr == "MySQL") return DatabaseType::M;
        if (params.dbTypeStr == "SQLite") return DatabaseType::S;
        throw std::invalid_argument("Unknown database type: " +
                                    std::string(params.dbTypeStr));
      }()) {
  if (name.empty()) {
    throw std::invalid_argument("Database configuration requires name.");
  }

  const bool has_credentials = (username.has_value() && !username->empty()) &&
                               (password.has_value() && !password->empty()) &&
                               (host.has_value() && !host->empty()) &&
                               port.has_value();
  const bool has_any_credential =
      (username.has_value() && !username->empty()) ||
      (password.has_value() && !password->empty()) ||
      (host.has_value() && !host->empty()) || port.has_value();

  switch (dbType) {
    case DatabaseType::P:
    case DatabaseType::M:
      if (!has_credentials) {
        throw std::invalid_argument(
            std::string(params.dbTypeStr) +
            " requires username, password, host, and port.");
      }
      break;

    case DatabaseType::S:
      if (has_any_credential) {
        throw std::invalid_argument(
            "SQLite configuration cannot contain username, password, host, or "
            "port.");
      }
      break;
  }
}

/* Getters*/

auto DatabaseConfig::getName() const -> const std::string& { return name; }
auto DatabaseConfig::getUserName() const -> const std::optional<std::string>& {
  return username;
}
auto DatabaseConfig::getPassword() const -> const std::optional<std::string>& {
  return password;
}
auto DatabaseConfig::getHost() const -> const std::optional<std::string>& {
  return host;
}
auto DatabaseConfig::getPort() const -> std::optional<int> { return port; }
auto DatabaseConfig::getDbType() const -> DatabaseType { return dbType; }
