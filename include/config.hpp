#pragma once
#include <optional>
#include <string>
#include <string_view>

class DatabaseConfig {
 public:
  enum class DatabaseType : char { P = 'P', M = 'M', S = 'S' };
  struct ConnectionParams {
    std::optional<std::string> username;
    std::optional<std::string> password;
    std::optional<std::string> host;
    std::optional<int> port;
    std::string name;
    std::string_view dbTypeStr;
  };

 private:
  std::optional<std::string> username;
  std::optional<std::string> password;
  std::optional<std::string> host;
  std::optional<int> port;
  std::string name;
  DatabaseType dbType;

 public:
  /* Initializer */
  explicit DatabaseConfig(ConnectionParams params);

  /* Getters */

  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getUserName() const -> const std::optional<std::string>&;
  [[nodiscard]] auto getPassword() const -> const std::optional<std::string>&;
  [[nodiscard]] auto getHost() const -> const std::optional<std::string>&;
  [[nodiscard]] auto getPort() const -> std::optional<int>;
  [[nodiscard]] auto getDbType() const -> DatabaseType;
};
