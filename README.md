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

## Parsing & Deserialization Logic

The project separates raw schema ingestion from business object instantiation across two specialized classes using the `nlohmann/json` backend:

1. **`Parser`** ([`include/parser.hpp`](include/parser.hpp)): Handles file-stream ingestion, structural formatting validation, and low-level inline constraints (e.g., port values matching `0–65535`).
2. **`Deserializer`** ([`include/deserializer.hpp`](include/deserializer.hpp)): Transforms loose, unstructured `json` objects into concrete `DatabaseConfig` instances.

### Empty String Sanitization

The deserialization pipeline automatically treats explicit empty JSON strings (`"username": ""`) as missing parameters. They are converted directly to `std::nullopt` before validation tracking fires, decoupling structural JSON edge cases from core domain safety guards.

### Example JSON configuration

`fixtures/normal_network.json`:

```json
{
  "username": "db_user",
  "password": "secure_pass_123",
  "host": "127.0.0.1",
  "port": 5432,
  "name": "metrics_warehouse",
  "db_type": "PostgresQL"
}
```

### Example Usage

```cpp
#include <iostream>
#include <filesystem>

#include "parser.hpp"
#include "deserializer.hpp"

const filesystem::path JSON_FILE{"fixtures/normal_network.json"};

int main() {
    try {
        // 1. Load file safely into memory
        nlohmann::json raw_data = Parser::load(JSON_FILE);

        // 2. Deserialize loose JSON directly into a validated C++ object
        DatabaseConfig config = Deserializer::from_json(raw_data);

        std::cout << "Successfully loaded: " << config.getName() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Configuration Error: " << e.what() << "\n";
        return 1;
    }
}
```

---

## Printer Logic

The `Printer` component ([`include/printer.hpp`](include/printer.hpp)) formats a `DatabaseConfig` instance into a human‑readable, aligned representation suitable for CLI output.

* **Label Width Customization** – Users can specify the maximum label width to align values neatly (minimum width is 16).
* **Pretty‑Printing** – Prints each field cleanly. To protect network layouts and local SQLite contexts, unpopulated optional fields are hidden from the console output entirely.
* **Extensible Output Stream** – Supports printing directly to `std::cout` by default, or to any custom output destination like standard file streams or string buffers.

```cpp
#include <iostream>
#include "printer.hpp"
#include "config.hpp"

int main() {
    DatabaseConfig cfg = /* ... load config ... */;

    // Initialize printer with a custom padding width of 18
    Printer printer_obj(18);

    // 1. Prints directly to std::cout using the default parameter argument
    printer_obj.printer(cfg);

    // 2. Redirect output to any valid standard stream reference safely
    std::ostream& out = std::clog;
    printer_obj.printer(cfg, out);

    return 0;
}
```

---

## Testing

There are tests provided in [`test/`](tests/) directory and can be run by `ctest`. For running tests:

```bash
# Make build/ directory
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release

# CMake to build project
cmake --build build --config Release

# Run tests via ctest
ctest --test-dir build -C Release --output-on-failure
```

---
