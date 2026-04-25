
# shark

> **Protobuf defines "what data is"  
> Shark Runtime defines "how data runs"**

---

## I. Core Goals

Shark is not just a code generator — it's a **Data Runtime System**.

It solves three core problems:

---

### 1️⃣ Eliminate Hand‑written Glue Code

Traditional data flow:

```text
proto → pb struct → business struct → JSON / DB / execution structure
```

Problems:

- Massive repetitive field copying
- Multiple layers of conversion (pb ↔ DTO ↔ domain)
- High maintenance cost
- Unpredictable performance

---

Shark's approach:

```text
proto → Runtime Class (auto‑generated + inheritable)
```

Features:

- Automatically generates:
    - getters / setters
    - proto ↔ runtime conversions
    - JSON ↔ runtime conversions
- Protected member variables (supports business extension)
- **Completely eliminates hand‑written conversion code**

---

### 2️⃣ Runtime Performance

Traditional way:

```text
pb → convert → struct → convert → JSON → convert → execution structure
```

👉 This is often the largest overhead in real‑world systems.

---

Shark's way:

```text
Runtime Object = Execution Object
```

Example:

```cpp
Person p;
p.age(42);
p.mutable_emails().push_back("alice@example.com");
```

> All of these operations use **compile‑time determined data access paths**:
> **no reflection, no intermediate objects, no extra copies**.

---

### 3️⃣ Bridging Proto with Columnar Storage (Core Direction)

The traditional problem:

- Protobuf → row‑based structure
- OLAP / vectorized execution → column‑based structure
- Requires massive reshaping in between

---

Shark's target:

```text
Proto Schema
   ↓
Runtime IR (unified representation)
   ↓
Row / Column / Vectorized execution
```

👉 Protobuf is no longer just an interface definition — it becomes:

> **A unified data representation IR (Intermediate Representation)**

---

## II. Core Design

---

### Layered Model

```text
[ Proto Layer ]        defines data structure (what)
        ↓
[ Runtime Layer ]      defines execution semantics (how)
        ↓
[ Execution Layer ]    query / compute / storage
```

---

### Runtime Type Design

Auto‑generated runtime class:

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

Key points:

- ✅ Not a pb wrapper
- ✅ Independent runtime object
- ✅ Supports inheritance (business extension)
- ✅ Can participate in execution (not just static data)

---

## III. Practical Examples (Full Capability)

---

### 1️⃣ Basic usage

```proto
message Person {
  string name = 1;
  int32 age = 2;
}
```

---

After generation:

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

### 4️⃣ Complex structures (real capability)

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

### 5️⃣ oneof (runtime implementation)

```cpp
p.set_aaa("hello");

if (p.has_aaa()) {
    std::cout << p.aaa();
}
```

---

Internal implementation (runtime‑level):

```cpp
union kind {
  std::unique_ptr<std::string> _aaa;
  uint32_t _bbb;
};
```

👉 This is **real memory layout**, not protobuf reflection.

---

### 6️⃣ Zero‑cost execution semantics (critical)

> All field access:
>
> * No reflection
> * No intermediate structures
> * No extra copies
> * Compile‑time determined access paths

---

## IV. Business Extension Capability (Core Value)

---

### Inheritance Extension

```cpp
class User : public Person {
public:
    bool is_adult() const {
        return age() >= 18;
    }
};
```

---

### Avoiding DTO Hell

Traditional:

```text
pb → dto → domain → view
```

Shark:

```text
runtime (single structure)
```

---

## V. Essential Differences from Traditional Protobuf

| Capability             | Protobuf | Shark |
| ---------------------- | -------- | ----- |
| Data definition        | ✅       | ✅    |
| Data manipulation      | ❌       | ✅    |
| Runtime expression     | ❌       | ✅    |
| Execution participation | ❌       | ✅    |
| Multi‑structure conversion | required | not needed |

---

> **Shark is not an enhancement of Protobuf — it elevates Protobuf into an executable data IR.**

---

## VI. CMake / kmcmake Integration

---

### 1️⃣ Generation workflow

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

### 2️⃣ Building a unified library

```cmake
kmcmake_cc_library(
    NAMESPACE ${PROJECT_NAME}
    NAME sk_test
    SOURCES
        ${test_sk_gen_SRCS}
        ${test_proto_gen_SRCS}
    PLINKS
        shark::shark_static
)
```

---

### 3️⃣ Testing

```cmake
kmcmake_cc_test(
    NAME test_proto_test
    SOURCES test_proto_test.cc
    LINKS
        shark::sk_test_static
        shark::shark_static
        GTest::gtest_main
)
```

---

### 4️⃣ Build flow summary

```text
.proto
  ↓ (kmcmake_cc_proto)
.pb.cc / .pb.h
  ↓ (kmcmake_cc_sk)
.runtime.cc / .runtime.h
  ↓ (kmcmake_cc_library)
Unified business library (sk_test)
  ↓
test / service
```

---

## VII. Build and Run

---

### Build

```bash
cmake --preset=default
cmake --build build -j
```

---

### Run tests

```bash
ctest --test-dir build
```

---

## VIII. Summary

Shark is not:

- A code generator
- A protobuf enhancement library

Instead:

```text
Proto (data definition)
    ↓
Runtime (execution semantics)
    ↓
Storage / Compute (system execution)
```

---

It solves:

- ❌ Hand‑written data conversion code
- ❌ Multi‑structure copying overhead
- ❌ Row / column fragmentation
- ❌ Protobuf not being able to participate in execution

---

Thus elevating Protobuf into:

> **A unified data representation and execution IR**

---

## IX. One‑Sentence Summary

> **Protobuf defines "what data is",  
> Shark defines "how data flows and how it is computed".**

---