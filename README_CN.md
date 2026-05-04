# Shark：数据定义与执行统一框架

[ENGLISH](README.md)

> **Protobuf 定义“数据是什么”，Shark 定义“数据如何运行、如何生成、如何校验、如何集成”。**

Shark 是一个 **数据运行时系统** + **TOML 配置代码生成器** + **配置集成框架**。

---

## 一、它能做什么？

1. **消除手写胶水代码**：`.proto` → 直接生成可继承的 C++ 运行时类，无需手写转换。
2. **零成本执行语义**：字段访问编译期确定，无反射、无中间拷贝。
3. **TOML 配置全生命周期管理**：生成解析/序列化代码，支持最小输出（仅 required 字段）、校验器、注释自动生成。
4. **配置集成**：通过 `import` 跨项目复用配置定义（如统一数据库连接配置）。
5. **打通 Proto 与列式存储**：Proto 作为统一 IR，支持行式/列式/向量化执行。

---

## 二、快速开始（单文件）

### 1. `gateway.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";

option (shark.idl.shark_file) = {
  runtime_namespace: "demo::gateway"
};

/// 网关配置
message GatewayConfig {
  required string service_name = 1;
  required int32 listen_port = 2 [default = 8080];
  optional string log_level = 3 [default = "INFO"];
}
```

### 2. CMake 集成（标准写法）

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

### 3. 使用生成的配置类

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

  // 只输出 required 字段
  auto minimal = cfg.serialize_required_to_string().value();
  std::cout << minimal << std::endl;
}
```

---

## 三、配置集成：跨项目复用（以 MariaDB 为例）

### 项目结构

```
common/
  mariadb_base.proto          # 公共配置
app/
  app_service.proto           # 业务配置，import "mariadb_base.proto"
  CMakeLists.txt
```

### 1. 公共库 `mariadb_base.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";

option (shark.idl.shark_file) = {
  runtime_namespace: "db::mariadb"
};

package db.mariadb;

/// MariaDB 连接配置
message Connection {
  required string host = 1;
  required int32 port = 2 [default = 3306];
  required string user = 3;
  required string password = 4;
  optional int32 max_connections = 5 [default = 10];
}
```

### 2. 业务服务 `app_service.proto`

```protobuf
syntax = "proto2";
import "shark/idl/shark_options.proto";
import "mariadb_base.proto";      // 跨项目引用

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

### 3. CMakeLists.txt（与 demo 写法一致）

```cmake
find_package(shark REQUIRED)

# 列出所有需要生成代码的 proto 文件（包括依赖的）
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

**说明**：
- 不需要设置任何额外的路径变量。
- 被依赖的 proto 文件（`mariadb_base.proto`）必须显式列在 `SHARK_IDL_TEST_FILES` 中。
- Shark 会自动分析 `import` 语句，生成正确的 `#include` 和跨命名空间调用。

### 4. 使用生成的配置（自动集成）

```cpp
#include "app_service.tml.h"     // 自动包含 mariadb_base.tml.h
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

## 四、业务扩展能力（继承）

生成的 Runtime 类支持继承，避免 DTO 地狱。

```cpp
class UserServiceConfig : public my::app::ServiceConfig {
public:
    bool is_using_replica() const {
        return replica_db.has_value();   // optional 字段使用 has_xxx 判断
    }
};
```

---

## 五、生成代码 API 概览

### 消息类（如 `ServiceConfig`）

| 方法                                                | 说明                         |
| --------------------------------------------------- | ---------------------------- |
| `parse_toml_str(const std::string&)`               | 从 TOML 字符串解析            |
| `parse_toml_file(const std::string&)`              | 从 TOML 文件解析              |
| `serialize_toml() const`                           | 全量序列化为 `xtoml::Value`   |
| `serialize_to_string() const`                      | 全量序列化为 TOML 字符串      |
| `serialize_required_toml() const`                  | 仅序列化 required 字段        |
| `serialize_required_to_string() const`             | 最小序列化为字符串            |
| `add_checker(std::unique_ptr<BasicHandler>)`       | 添加字段校验器                |

字段访问：所有字段为 `public` 成员变量，直接读写。

### 枚举（如 `LogLevel`）

```cpp
std::string_view to_string(LogLevel value);
std::optional<LogLevel> parse_LogLevel(std::string_view str);
```

---

## 六、支持的类型与明确限制

### 支持类型

- 整数（int32/int64/uint32/uint64/sint32/sint64/fixed32/fixed64/sfixed32/sfixed64）
- 浮点数（float/double）
- bool
- string
- enum（映射为字符串，区分大小写）
- message（嵌套表）
- repeated 数组（基础类型/枚举/消息）
- **跨文件顶层消息引用** (`import`)

### 不支持 / 禁止

| 特性                           | 处理方式                              |
| ------------------------------ | ------------------------------------- |
| `google.protobuf.Any`          | 不支持                                |
| `map`                          | 需手动用 `repeated message` 模拟       |
| `oneof`                        | 不支持（业务层处理）                  |
| `bytes`                        | 存 base64 字符串                      |
| 跨消息引用嵌套消息（如 `Outer.Inner`） | 禁止，需将嵌套消息提升为顶层          |

**跨文件集成的限制**：  
只能引用其他文件中的 **顶层消息**，不能引用嵌套在另一消息内部的类型。这是为了保证集成路径的清晰和构建依赖的可预测性。

---

## 七、校验器机制

### 内置校验器

| 校验器                    | 功能                         |
| ------------------------- | ---------------------------- |
| `RangeHandler<T>`         | 数值区间（闭/开）             |
| `StringHandler`           | 长度、正则、前缀/后缀、白名单 |
| `InSetHandler<T>`         | 集合成员检查（黑/白名单）     |
| `CustomHandler<T>`        | 任意 lambda 校验              |
| `RepeatedHandler<Container>` | 数组元素级 + 容器级校验      |

### 使用示例

```cpp
// 限制 MariaDB 端口在 1024～65535 之间
auto uri = xtoml::TomlUri::create("primary_db.port").value();
auto checker = xtoml::RangeHandler<int32_t>::create(
    uri, &cfg.primary_db.port, false, 3306, 1024, 65535);
cfg.add_checker(std::move(checker));
```

校验失败时返回带字段路径的错误信息，例如：
`Field 'primary_db.port': value 80 must be >= 1024`

---

## 八、注释 → TOML 文档

Proto 中的 `///` 三斜杠注释会作为 TOML 字段的注释块输出。

```protobuf
/// Port for the database connection.
    required int32 port = 2 [default = 3306];
```

生成 TOML：

```
#############################################
# Port for the database connection.
### end
port = 3306
```

---

## 九、CMake 多文件依赖示例（另一个完整 demo）

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

## 十、与传统 Protobuf 的区别

- **数据操作**：protobuf 只定义，不操作；Shark 定义 + 操作合一。
- **性能**：protobuf 访问有拷贝/反射开销；Shark 为编译期直接内存访问。
- **扩展**：protobuf 不可继承；Shark 支持业务继承。
- **配置生成**：protobuf 需要手写解析；Shark 直接输出 TOML 处理代码。
- **配置集成**：protobuf 只提供类型引用；Shark 提供跨项目、可发布的配置复用机制。

---

## 十一、总结

> **Protobuf 解决“数据是什么”，Shark 解决“数据如何流动、如何生成、如何校验、如何集成”。**

Shark 统一了：

- 配置定义（proto）→ 自动生成 TOML 解析/序列化代码
- 运行时数据对象（零成本访问、支持继承）
- 校验机制（声明式或编程式）
- 配置集成（跨项目 `import` 复用，CMake 只需列出所有 proto 文件）
- 执行语义准备（为列式/向量化执行打下基础）

**无需手写转换代码，无需维护多套结构，一步从定义到可运行。**

更多示例请参考：
- `examples/` – 完整网关配置演示（含跨文件集成）
- `tests/require/` – 单元测试覆盖所有特性

**许可证**：Apache 2.0  
**维护者**：Kumo Inc.