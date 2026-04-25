encoding
=====================================

## Shark SKP Layout Encoding 规范（基于 Cap'n Proto 内存模型）
**适用**：Protobuf Message → 零拷贝二进制序列化/反序列化
**核心**：无 tag、无 varint、无压缩 → 纯内存布局
**依赖**：仅需 Protobuf Descriptor（无需任何 Cap'n Proto 生成文件）

---

## 1. 基础定义
### 1.1 基本单位
```
Word = 8 字节（64bit）
所有数据必须按 Word 对齐
所有偏移、长度、指针 → 单位都是 Word
```

### 1.2 支持的 Protobuf 类型全覆盖
- `optional / required`
- `repeated`
- `oneof`
- `enum`（可指定 u8/u16/u32）
- `map`
- `google.protobuf.Any`
- nested message
- string / bytes

---

## 2. Struct 布局（Message 本体）
### 2.1 结构
```
[ Struct ] = N 个连续 Word
N = 字段数量（来自 descriptor->field_count()）
```

### 2.2 字段顺序
**严格按照 Protobuf Descriptor 字段定义顺序**
```
field 0 → word 0
field 1 → word 1
field 2 → word 2
...
```

### 2.3 字段存储规则
- **值类型**：直接内联在 word
- **指针类型**：word 存储 Pointer（指向外部数据）
- **oneof**：整个 oneof 占用 1 个 word

---

## 3. Pointer 编码（最核心）
### 3.1 格式（1 Word = 64bit）
```
 63 ~ 32 bit: offset (相对偏移，Word 数量)
 31 ~ 16 bit: type
 15 ~  0 bit: length
```

### 3.2 type 定义
```
0 = struct
1 = list
2 = string/bytes
3 = any
6 = oneof (union)
```

### 3.3 offset 规则
```
offset = 目标地址 - 当前指针地址（按 Word 计算）
```
**相对偏移 → 可跨进程 / mmap / 零拷贝**

---

## 4. 各类型布局规则

### 4.1 数值类型（int32/uint32/bool/enum）
```
直接内联在 struct word
```
- enum：按指定底层类型（u8/u16/u32）存储
- bool：1 byte，其余填充 0

## 4.2 string / bytes
```
struct word → Pointer(type=2)
Pointer 指向：
  [length][byte0][byte1]...][padding to 8]
```

### 4.3 repeated 字段
```
struct word → Pointer(type=1)
Pointer 指向：
  [size][element0][element1]...][padding]
```
- 每个元素 1 Word
- size = 元素个数

### 4.4 oneof
```
整个 oneof 占用 1 个 Word
格式：
  [tag: 16bit][data: 48bit]
```
- tag：激活字段的 index
- data：内联小数据 / 指针

### 4.5 nested message
```
struct word → Pointer(type=0)
Pointer 指向子 struct（N 个 Word）
```

## 4.6 map<string, int32>
```
map = repeated<Entry>
Entry = { key: xx, value: xx }
```
最终表现为 list of struct

## 4.7 google.protobuf.Any
```
struct word → Pointer(type=3)
存储：
  type_url: string
  value: bytes(内嵌消息)
```

---

# 5. 内存段规则
## 5.1 所有内存从 Arena 分配
## 5.2 多段消息支持
## 5.3 最终序列化 = Arena 内存直接输出

---

# 6. 示例布局（你给的 Person）
```protobuf
message Person {
  optional string name = 1;      // word 0 (pointer)
  optional int32 age = 2;         // word 1 (value)
  repeated string emails = 3;      // word 2 (pointer → list)
  map<string, int32> scores = 4;  // word 3 (pointer → list)
  optional Color favorite_color =5; // word4 (value)
  oneof kind {                    // word5 (oneof)
    string aaa = 10;
    uint32 bbb = 12;
  }
  optional Address address =6;     // word6 (pointer → struct)
}
```

**内存布局**
```
word0: name (ptr)
word1: age (i32)
word2: emails (ptr → list)
word3: scores (ptr → list)
word4: color (enum)
word5: oneof (tag+data)
word6: address (ptr → struct)
```

---

# 7. 编码流程（入口）
## 7.1 入口
```cpp
void encode(const google::protobuf::Message& pb_msg, Arena& arena, Blob& output);
```

## 7.2 步骤
1. 获取 descriptor
2. 计算 struct word 数量
3. 分配 struct 内存
4. 遍历所有字段
   - 值类型 → 直接写
   - 指针类型 → 分配数据段 → 计算 offset → 写 pointer
5. 输出 arena 内存

---

# 8. 反序列化流程
```cpp
void decode(const Blob& blob, google::protobuf::Message& pb_msg);
```
- 直接解析内存
- 按 layout 读取字段
- 0 拷贝

---

# 你要我下一步做什么？
我可以**直接输出**：
1. **完整 C++ 头文件：layout_encoding.h**
2. **完整编码实现：layout_encoding.cc**
3. **可直接跑的示例：把你的 Person.pb 零拷贝序列化**

你要我**直接写代码实现**吗？