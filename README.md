# JSON Configuration Parser

A modern C++20 utility designed to ingest JSON configuration profiles, validate their structural properties against a deterministic schema, and deserialize them into type-safe C++ runtime environments.

## Features

* **Schema Validation**: Hard validation bounds on required database parameters.
* **C++20 Type Safety**: Strongly-typed structures preventing "zombie state" instantiations.
* **CLI Engine Ready**: Configured for clean CLI queries and pretty-printing pipelines.

---

## Configuration Specifications

The central operational unit is the `DatabaseConfig` class, declared within [`include/config.hpp`](include/config.hpp). It maps structural JSON configuration fields directly into type-safe C++ structures.

### Field Definitions

| Value      | C++ Type                     | Constraints / Description                                                      | Example                        |
| ---------- | ---------------------------- | ------------------------------------------------------------------------------ | ------------------------------ |
| `name`     | `std::string`                | **Required**. Cannot be empty. Target system name.                             | `"production_db"`              |
| `dbType`   | `enum class`                 | **Required**. Validated engine type mapping (`P`, `M`, `S`).                   | `DatabaseType::P`              |
| `username` | `std::optional<std::string>` | Required for Network DBs. Illegal on SQLite.                                   | `"admin"` / `std::nullopt`     |
| `password` | `std::optional<std::string>` | Required for Network DBs. Illegal on SQLite.                                   | `"secret"` / `std::nullopt`    |
| `host`     | `std::optional<std::string>` | Required for Network DBs. Illegal on SQLite.                                   | `"127.0.0.1"` / `std::nullopt` |
| `port`     | `std::optional<int>`         | Required for Network DBs (between 0-65535). Illegal on SQLite.                 | `5432` / `std::nullopt`        |

### Database Engine Mappings

During instantiation, the parser processes a text-based database identifier (`dbTypeStr`) and resolves it into a strict internal `DatabaseType` enum token:

| Input JSON String (`dbTypeStr`) | Resolved Internal Enum Value | Architecture Strategy                                         |
| ------------------------------- | ---------------------------- | ------------------------------------------------------------- |
| `"PostgresQL"`                  | `DatabaseType::P`            | Network-Based (Requires network variables)                    |
| `"MySQL"`                       | `DatabaseType::M`            | Network-Based (Requires network variables)                    |
| `"SQLite"`                      | `DatabaseType::S`            | Local File/Memory-Based (Strictly excludes network variables) |

### Example

Configurations are instantiated safely by packaging properties inside an aggregate initialization struct:

```cpp
#include "config.hpp"

// Instantiating a Network-based PostgresQL Configuration
DatabaseConfig network_config(DatabaseConfig::ConnectionParams{
    .username  = "db_user",
    .password  = "secure_pass_123",
    .host      = "127.0.0.1",
    .port      = 5432,
    .name      = "metrics_warehouse",
    .dbTypeStr = "PostgresQL"
});

// Instantiating a Local SQLite Configuration
DatabaseConfig local_config(DatabaseConfig::ConnectionParams{
    .name      = "cache_store",
    .dbTypeStr = "SQLite"
});
```

---
