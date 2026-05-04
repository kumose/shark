# Shark: Unified Framework for Data Definition and Execution

[中文版](README_CN.md)

> **Protobuf defines "what data is", Shark defines "how data flows, how it is generated, how it is validated, and how it integrates."**

Shark is a **Data Runtime System** + **TOML Configuration Code Generator** + **Configuration Integration Framework**.

---

## 1. What It Can Do

1. **Eliminate handwritten glue code**: `.proto` → directly generate inheritable C++ runtime classes, no manual conversions.
2. **Zero‑cost execution semantics**: field access is determined at compile‑time, no reflection, no intermediate copies.
3. **Full TOML configuration lifecycle**: generate parsing/serialization code, support minimal output (required‑only fields), validators, automatic comment mapping.
4. **Configuration integration**: reuse configuration definitions across projects via `import` (e.g., unified database connection config).
5. **Bridge between Proto and columnar storage**: Proto becomes a unified IR, supporting row‑based, column‑based, and vectorized execution.

---

## 2. Quick Start (single file)

### 2.1 `gateway.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";

option (shark.idl.shark_file) = {
  runtime_namespace: "demo::gateway"
};

/// Gateway configuration
message GatewayConfig {
  required string service_name = 1;
  required int32 listen_port = 2 [default = 8080];
  optional string log_level = 3 [default = "INFO"];
}
```

### 2.2 CMake integration (standard)

```cmake
find_package(shark REQUIRED)

set(SHARK_IDL_TEST_FILES
    gateway.proto
)

kmcmake_cc_tml(
    NAME gateway_gen
    PROTOS ${SHARK_IDL_TEST_FILES}
    OUTDIR ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_binary(
    NAMESPACE ${PROJECT_NAME}
    NAME gateway_demo
    SOURCES ${gateway_gen_SRCS} main.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    LINKS ${KMCMAKE_DEPS_LINK}
)
```

### 2.3 Using the generated configuration class

```cpp
#include "gateway.tml.h"
#include <iostream>

int main() {
  demo::gateway::GatewayConfig cfg;

  std::string toml = R"(
    service_name = "my-api"
    listen_port = 9090
  )";
  if (!cfg.parse_toml_str(toml).ok()) return 1;

  std::cout << cfg.service_name << std::endl;   // "my-api"
  std::cout << cfg.listen_port << std::endl;    // 9090
  std::cout << cfg.log_level << std::endl;      // "INFO" (default)

  // Minimal output (required fields only)
  auto minimal = cfg.serialize_required_to_string().value();
  std::cout << minimal << std::endl;
}
```

---

## 3. Configuration Integration: Cross‑Project Reuse (MariaDB example)

### Project layout

```
common/
  mariadb_base.proto          # base configuration
app/
  app_service.proto           # business configuration, imports "mariadb_base.proto"
  CMakeLists.txt
```

### 3.1 Base library `mariadb_base.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";

option (shark.idl.shark_file) = {
  runtime_namespace: "db::mariadb"
};

package db.mariadb;

/// MariaDB connection settings
message Connection {
  required string host = 1;
  required int32 port = 2 [default = 3306];
  required string user = 3;
  required string password = 4;
  optional int32 max_connections = 5 [default = 10];
}
```

### 3.2 Business service `app_service.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";
import "mariadb_base.proto";      // cross‑project import

option (shark.idl.shark_file) = {
  runtime_namespace: "my::app"
};

package my.app;

message ServiceConfig {
  required string service_name = 1;
  required db.mariadb.Connection primary_db = 2;
  optional db.mariadb.Connection replica_db = 3;
}
```

### 3.3 CMakeLists.txt (same style as demo)

```cmake
find_package(shark REQUIRED)

# List all .proto files that need code generation (including dependencies)
set(SHARK_IDL_TEST_FILES
    mariadb_base.proto
    app_service.proto
)

kmcmake_cc_tml(
    NAME app_config_gen
    PROTOS ${SHARK_IDL_TEST_FILES}
    OUTDIR ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_binary(
    NAMESPACE ${PROJECT_NAME}
    NAME app_demo
    SOURCES ${app_config_gen_SRCS} main.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    LINKS ${KMCMAKE_DEPS_LINK}
)
```

**Remarks**:
- No extra path variables are required.
- Dependent `.proto` files must be explicitly listed in `SHARK_IDL_TEST_FILES`.
- Shark automatically analyses `import` statements and generates correct `#include` and cross‑namespace calls.

### 3.4 Using the generated configuration (automatic integration)

```cpp
#include "app_service.tml.h"     // automatically includes mariadb_base.tml.h
#include <iostream>

int main() {
  my::app::ServiceConfig cfg;

  std::string toml = R"(
    service_name = "user-service"
    [primary_db]
    host = "mariadb.example.com"
    port = 3306
    user = "app_user"
    password = "secure"
  )";

  if (!cfg.parse_toml_str(toml).ok()) return 1;

  std::cout << "Primary DB: " << cfg.primary_db.host
            << ":" << cfg.primary_db.port << std::endl;
}
```

---

## 4. Business Extension (Inheritance)

Generated runtime classes support inheritance, avoiding DTO hell.

```cpp
class UserServiceConfig : public my::app::ServiceConfig {
public:
    bool is_using_replica() const {
        return replica_db.has_value();   // optional fields use has_xxx()
    }
};
```

---

## 5. Generated API Overview

### Message class (e.g., `ServiceConfig`)

| Method                                                | Description                                   |
| ----------------------------------------------------- | --------------------------------------------- |
| `parse_toml_str(const std::string&)`               | Parse from a TOML string                      |
| `parse_toml_file(const std::string&)`              | Parse from a TOML file                        |
| `serialize_toml() const`                           | Full serialization to `xtoml::Value`          |
| `serialize_to_string() const`                      | Full serialization to TOML string             |
| `serialize_required_toml() const`                  | Serialize only required fields (recursive)    |
| `serialize_required_to_string() const`             | Minimal serialization to TOML string          |
| `add_checker(std::unique_ptr<BasicHandler>)`       | Add a field validator                         |

Field access: all fields are `public` member variables, read/write directly.

### Enum (e.g., `LogLevel`)

```cpp
std::string_view to_string(LogLevel value);
std::optional<LogLevel> parse_LogLevel(std::string_view str);
```

---

## 6. Supported Types and Explicit Limitations

### Supported types

- Integers (`int32/int64/uint32/uint64/sint32/sint64/fixed32/fixed64/sfixed32/sfixed64`)
- Floating‑point (`float/double`)
- `bool`
- `string`
- `enum` (mapped to strings, case‑sensitive)
- `message` (nested tables)
- `repeated` arrays (of primitive types, enums, or messages)
- **Cross‑file top‑level message references** (`import`)

### Unsupported / forbidden

| Feature                         | Handling                                       |
| ------------------------------- | ---------------------------------------------- |
| `google.protobuf.Any`          | Not supported                                  |
| `map`                          | Manually simulate with `repeated message`     |
| `oneof`                        | Not supported (handle in business logic)      |
| `bytes`                        | Use base64‑encoded string instead             |
| Cross‑message nested message references (e.g., `Outer.Inner`) | Forbidden – lift the nested message to top‑level |

**Cross‑file integration restriction**:  
Only **top‑level messages** from other files can be referenced; nested messages inside another message are not allowed. This ensures clean integration paths and predictable build dependencies.

---

## 7. Validator Mechanism

### Built‑in validators

| Validator                     | Purpose                                             |
| ----------------------------- | --------------------------------------------------- |
| `RangeHandler<T>`             | Numeric range (closed/open intervals)              |
| `StringHandler`               | Length, regex, prefix/suffix, white‑list           |
| `InSetHandler<T>`             | Set membership check (allow/deny list)             |
| `CustomHandler<T>`            | Arbitrary lambda validator                         |
| `RepeatedHandler<Container>`  | Element‑wise + container‑level validation          |

### Usage example

```cpp
// Restrict MariaDB port to 1024‑65535
auto uri = xtoml::TomlUri::create("primary_db.port").value();
auto checker = xtoml::RangeHandler<int32_t>::create(
    uri, &cfg.primary_db.port, false, 3306, 1024, 65535);
cfg.add_checker(std::move(checker));
```

On failure, an error containing the field path is returned, e.g.:
`Field 'primary_db.port': value 80 must be >= 1024`

---

## 8. Comments → TOML Documentation

Triple‑slash (`///`) English comments in the `.proto` file are automatically extracted and become field comments in the generated TOML output.

```protobuf
/// Port for the database connection.
required int32 port = 2 [default = 3306];
```

Generated TOML:

```
#############################################
# Port for the database connection.
### end
port = 3306
```

---

## 9. CMake Example with Multiple Files (Another complete demo)

```cmake
find_package(shark REQUIRED)

set(SHARK_IDL_TEST_FILES
    common.proto
    service.proto
    types.proto
)

kmcmake_cc_tml(
    NAME my_config
    PROTOS ${SHARK_IDL_TEST_FILES}
    OUTDIR ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_binary(
    NAMESPACE ${PROJECT_NAME}
    NAME my_app
    SOURCES ${my_config_SRCS} main.cc
    CXXOPTS ${KMCMAKE_CXX_OPTIONS}
    LINKS ${KMCMAKE_DEPS_LINK}
)
```

---

## 10. Differences from Traditional Protobuf

- **Data manipulation**: Protobuf only defines data; Shark integrates definition and manipulation.
- **Performance**: Protobuf access involves copying/reflection; Shark provides compile‑time direct memory access.
- **Extensibility**: Protobuf does not support inheritance; Shark supports business inheritance.
- **Configuration generation**: Protobuf requires handwritten parsing; Shark directly outputs TOML‑ready code.
- **Configuration integration**: Protobuf only provides type referencing; Shark provides cross‑project, reusable configuration integration.

---

## 11. Summary

> **Protobuf solves "what data is", Shark solves "how data flows, how it is generated, how it is validated, how it integrates".**

Shark unifies:

- Configuration definition (proto) → automatically generate TOML parsing/serialization code
- Runtime data objects (zero‑cost access, inheritance support)
- Validation mechanisms (declarative or programmatic)
- Configuration integration (cross‑project `import` reuse, CMake only needs to list all proto files)
- Execution‑semantics preparation (paving the way for columnar/vectorised execution)

**No handwritten conversion code, no maintaining multiple representations, one step from definition to runnable code.**

More examples:
- `examples/` – complete gateway configuration demo (including cross‑file integration)
- `tests/require/` – unit tests covering all features

**License**: Apache 2.0  
**Maintainer**: Kumo Inc.