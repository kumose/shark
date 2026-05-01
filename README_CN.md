# shark

> **Protobuf 定义“数据是什么”
> Shark Runtime 定义“数据如何运行”**

---

## 一、项目核心目标

shark 不是一个简单的代码生成工具，而是一个**数据运行时系统（Data Runtime System）**。

它解决三个核心问题：

---

### 1️⃣ 消除手写胶水代码

传统数据流：

```text
proto → pb struct → 业务 struct → JSON / DB / 执行结构
```

问题：

* 大量重复字段拷贝
* 多层结构转换（pb ↔ dto ↔ domain）
* 维护成本极高
* 性能不可控

---

shark 的方式：

```text
proto → Runtime Class（自动生成 + 可继承）
```

特点：

* 自动生成：

  * getter / setter
  * proto ↔ runtime
  * json ↔ runtime
* 成员变量为 `protected`（支持业务继承）
* **彻底消除手写转换代码**

---

### 2️⃣ 解决执行期性能问题

传统方式：

```text
pb → 转换 → struct → 转换 → json → 转换 → 执行结构
```

👉 实际系统中，这部分往往是最大开销来源

---

shark 的方式：

```text
Runtime Object = Execution Object
```

示例：

```cpp
Person p;
p.age(42);
p.mutable_emails().push_back("alice@example.com");
```

> 上述所有操作均为 **编译期确定的数据访问路径**，
> **无反射、无中间对象、无额外拷贝**。

---

### 3️⃣ 打通 Proto 与列式存储（核心方向）

传统问题：

* Protobuf → 行式结构（row-based）
* OLAP / 向量执行 → 列式结构（column-based）
* 中间需要大量 reshape

---

shark 目标：

```text
Proto Schema
   ↓
Runtime IR（统一表达）
   ↓
Row / Column / Vectorized 执行
```

👉 Proto 不再只是接口定义，而是：

> **统一的数据表达 IR（Intermediate Representation）**

---

## 二、核心设计

---

### 分层模型

```text
[ Proto Layer ]        定义数据结构（what）
        ↓
[ Runtime Layer ]      定义执行语义（how）
        ↓
[ Execution Layer ]    查询 / 计算 / 存储
```

---

### Runtime 类型设计

自动生成的 Runtime 类：

```cpp
class Person {
public:
  using proto_type = test::pb::pa::Person;

  void parse_from_proto(const proto_type& pb);
  void serialize_to_proto(proto_type& pb) const;

  turbo::Status parse_from_json(const std::string&);
  turbo::Status serialize_to_json(std::string&) const;

protected:
  std::string _name;
  int8_t _age;
};
```

---

关键点：

* ✅ 不是 pb wrapper
* ✅ 是独立运行时对象
* ✅ 支持继承（业务扩展）
* ✅ 可参与执行（不是静态数据结构）

---

## 三、实际示例（完整能力）

---

### 1️⃣ 基本使用

```proto
message Person {
  string name = 1;
  int32 age = 2;
}
```

---

生成后：

```cpp
Person p;
p.set_name("Alice");
p.age(42);
```

---

### 2️⃣ Proto ↔ Runtime

```cpp
test::pb::pa::Person pb;
p.serialize_to_proto(pb);

Person p2;
p2.parse_from_proto(pb);
```

---

### 3️⃣ JSON ↔ Runtime

```cpp
std::string json;
p.serialize_to_json(json);

Person p3;
p3.parse_from_json(json);
```

---

### 4️⃣ 复杂结构（真实能力）

```cpp
p.mutable_emails().push_back("alice@example.com");

absl::flat_hash_map<std::string, int32_t> scores;
scores["math"] = 95;
p.set_scores(scores);

Person::Address addr;
addr.set_street("Main St");
p.set_address(addr);
```

---

### 5️⃣ oneof（运行时实现）

```cpp
p.set_aaa("hello");

if (p.has_aaa()) {
    std::cout << p.aaa();
}
```

---

内部实现（runtime-level）：

```cpp
union kind {
  std::unique_ptr<std::string> _aaa;
  uint32_t _bbb;
};
```

👉 不是 protobuf reflection，而是**真实内存结构**

---

### 6️⃣ 零成本执行语义（关键）

> 所有字段访问：
>
> * 无反射
> * 无中间结构
> * 无多余拷贝
> * 编译期确定访问路径

---

## 四、业务扩展能力（核心价值）

---

### 继承扩展

```cpp
class User : public Person {
public:
    bool is_adult() const {
        return age() >= 18;
    }
};
```

---

### 避免 DTO 地狱

传统：

```text
pb → dto → domain → view
```

shark：

```text
runtime（唯一结构）
```

---

## 五、与传统 Protobuf 的本质区别

| 能力    | Protobuf | Shark |
| ----- | -------- | ----- |
| 数据定义  | ✅        | ✅     |
| 数据操作  | ❌        | ✅     |
| 运行时表达 | ❌        | ✅     |
| 执行参与  | ❌        | ✅     |
| 多结构转换 | 必须       | 不需要   |

---

> **Shark 不是对 Protobuf 的增强，
> 而是把 Protobuf 提升为“可执行数据 IR”。**

---

## 六、CMake / kmcmake 集成

---

### 1️⃣ 生成流程

```cmake
set(SHARK_IDL_TEST_FILES
    test.proto
)

kmcmake_cc_proto(
    NAME test_proto_gen
    PROTOS ${SHARK_IDL_TEST_FILES}
    OUTDIR ${PROJECT_SOURCE_DIR}
)

kmcmake_cc_sk(
    NAME test_sk_gen
    PROTOS ${SHARK_IDL_TEST_FILES}
    OUTDIR ${PROJECT_SOURCE_DIR}
)
```

---

### 2️⃣ 构建统一库

```cmake
kmcmake_cc_library(
    NAMESPACE ${PROJECT_NAME}
    NAME sk_test
    SOURCES
        ${test_sk_gen_SRCS}
        ${test_proto_gen_SRCS}
    PLINKS
        
)
```

---

### 3️⃣ 测试

```cmake
kmcmake_cc_test(
    NAME test_proto_test
    SOURCES test_proto_test.cc
    LINKS
        shark::sk_test_static
        
        GTest::gtest_main
)
```

---

### 4️⃣ 构建流程总结

```text
.proto
  ↓ (kmcmake_cc_proto)
.pb.cc / .pb.h
  ↓ (kmcmake_cc_sk)
.runtime.cc / .runtime.h
  ↓ (kmcmake_cc_library)
统一业务库（sk_test）
  ↓
test / service
```

---

## 七、构建与运行

---

### 构建

```bash
cmake --preset=default
cmake --build build -j
```

---

### 运行测试

```bash
ctest --test-dir build
```

---

## 八、总结

shark 的本质不是：

* 代码生成工具
* protobuf 增强库

而是：

```text
Proto（数据定义）
    ↓
Runtime（执行语义）
    ↓
Storage / Compute（系统执行）
```

---

它解决了：

* ❌ 手写数据转换代码
* ❌ 多结构复制开销
* ❌ row / column 割裂
* ❌ protobuf 无法参与执行

---

最终将 Protobuf 提升为：

> **统一的数据表达与执行 IR**

---

## 九、一句话总结

> **Protobuf 解决“数据是什么”，
> Shark 解决“数据如何流动、如何计算”。**

---
