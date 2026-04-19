// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <shark/generator/type.h>
#include <shark/ast/cpp_type.h>
#include <shark/ast/cpp_template.h>

namespace shark::generator {
    Type Type::void_type() {
        Type t;
        t.kind_ = Kind::k_void;
        return t;
    }

    Type Type::bool_type() {
        Type t;
        t.kind_ = Kind::k_bool;
        return t;
    }

    Type Type::int32_type() {
        Type t;
        t.kind_ = Kind::k_int32;
        return t;
    }

    Type Type::int64_type() {
        Type t;
        t.kind_ = Kind::k_int64;
        return t;
    }

    Type Type::uint32_type() {
        Type t;
        t.kind_ = Kind::k_uint32;
        return t;
    }

    Type Type::uint64_type() {
        Type t;
        t.kind_ = Kind::k_uint64;
        return t;
    }

    Type Type::float_type() {
        Type t;
        t.kind_ = Kind::k_float;
        return t;
    }

    Type Type::double_type() {
        Type t;
        t.kind_ = Kind::k_double;
        return t;
    }

    Type Type::string_type() {
        Type t;
        t.kind_ = Kind::k_string;
        return t;
    }

    Type Type::vector_type(Type element) {
        Type t;
        t.kind_ = Kind::k_vector;
        t.parameters_.push_back(std::move(element));
        return t;
    }

    Type Type::unordered_map_type(Type key, Type value) {
        Type t;
        t.kind_ = Kind::k_unordered_map;
        t.parameters_.push_back(std::move(key));
        t.parameters_.push_back(std::move(value));
        return t;
    }

    Type Type::user_defined(std::string name) {
        Type t;
        t.kind_ = Kind::k_user_defined;
        t.name_ = std::move(name);
        return t;
    }

    Type Type::enum_type(std::string name) {
        Type t;
        t.kind_ = Kind::k_enum;
        t.name_ = std::move(name);
        return t;
    }

    std::unique_ptr<cppast::CppType> Type::build() const {
        using namespace cppast;

        switch (kind_) {
            case Kind::k_void:
                return CppBuiltinType::build(cpp_void);
            case Kind::k_bool:
                return CppBuiltinType::build(cpp_bool);
            case Kind::k_int32:
                return CppBuiltinType::build(cpp_int);
            case Kind::k_int64:
                return CppBuiltinType::build(cpp_longlong);
            case Kind::k_uint32:
                return CppBuiltinType::build(cpp_uint);
            case Kind::k_uint64:
                return CppBuiltinType::build(cpp_ulonglong);
            case Kind::k_float:
                return CppBuiltinType::build(cpp_float);
            case Kind::k_double:
                return CppBuiltinType::build(cpp_double);
            case Kind::k_string:
                return CppUserDefinedType::build(CppTypeRef(CppEntityId("std::string"), "std::string"));
            case Kind::k_vector: {
                auto elem = parameters_[0].build();
                auto vec = CppTemplateInstantiationType::builder(
                    CppTemplateRef(CppEntityId("std::vector"), "std::vector"));
                vec.add_argument(CppTemplateArgument(std::move(elem)));
                return vec.finish();
            }
            case Kind::k_unordered_map: {
                // 确保 parameters_ 有两个元素：key 和 value
                if (parameters_.size() != 2) {
                    // 错误处理，返回 void 类型
                    return cppast::CppBuiltinType::build(cppast::cpp_void);
                }
                auto key_type = parameters_[0].build();
                auto value_type = parameters_[1].build();
                // 创建 std::unordered_map<Key, Value>
                auto map_template = cppast::CppTemplateInstantiationType::builder(
                    cppast::CppTemplateRef(cppast::CppEntityId("std::unordered_map"), "std::unordered_map"));
                map_template.add_argument(cppast::CppTemplateArgument(std::move(key_type)));
                map_template.add_argument(cppast::CppTemplateArgument(std::move(value_type)));
                return map_template.finish();
            }
            case Kind::k_user_defined:
                return CppUserDefinedType::build(CppTypeRef(CppEntityId(name_), name_));
            case Kind::k_enum:
                return CppUserDefinedType::build(CppTypeRef(CppEntityId(name_), name_));
        }
        return nullptr;
    }
} // namespace shark::generator
