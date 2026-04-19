// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
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


#include <shark/type.h>
#include <cstddef>
namespace cppast {

    TypeRegistry::TypeRegistry() {
        init_builtin();
        init_cstd();
    }

    void TypeRegistry::init_builtin() {
        _includes["void"] = "";
        _includes["bool"] = "";
        _includes["unsigned char"] = "";
        _includes["unsigned short"] = "";
        _includes["unsigned int"] = "";
        _includes["unsigned long"] = "";
        _includes["unsigned long long"] = "";
        _includes["unsigned __int128"] = "";
        _includes["signed char"] = "";
        _includes["short"] = "";
        _includes["int"] = "";
        _includes["long"] = "";
        _includes["long long"] = "";
        _includes["__int128"] = "";
        _includes["float"] = "";
        _includes["double"] = "";
        _includes["long double"] = "";
        _includes["__float128"] = "";
        _includes["char"] = "";
        _includes["wchar_t"] = "";
        _includes["char16_t"] = "";
        _includes["char32_t"] = "";
        _includes["decltype(nullptr)"] = "";
        _includes["std::nullptr_t"] = "";
    }

    void TypeRegistry::init_cstd() {
        _includes["int8_t"] = "cstdint";
        _includes["int16_t"] = "cstdint";
        _includes["int32_t"] = "cstdint";
        _includes["int64_t"] = "cstdint";
        _includes["uint8_t"] = "cstdint";
        _includes["uint16_t"] = "cstdint";
        _includes["uint32_t"] = "cstdint";
        _includes["uint64_t"] = "cstdint";
        _includes["size_t"] = "cstddef";
        _includes["max_align_t"] = "cstddef";
        _includes["std::max_align_t"] = "cstddef";
    }

    void TypeRegistry::init_stl() {
        _includes["std::string"] = "string";
        _includes["std::string_view"] = "string_view";
        _includes["std::basic_string"] = "string";
        _includes["std::basic_string_view"] = "string_view";
        _includes["std::list"] = "list";
        _includes["std::vector"] = "vector";
        _includes["std::deque"] = "deque";
        _includes["std::map"] = "map";
        _includes["std::set"] = "set";
        _includes["std::unordered_map"] = "unordered_map";
        _includes["std::unordered_set"] = "unordered_set";
        _includes["std::variant"] = "variant";
        _includes["std::optional"] = "optional";
        _includes["std::array"] = "array";
        _includes["std::any"] = "any";
    }

    bool TypeRegistry::has_type(std::string name) {
        auto it = _includes.find(name);
        return it != _includes.end();
    }

    void TypeRegistry::register_type(std::string name, std::string include) {
        _includes[name] = include;
    }

    std::optional<std::string> TypeRegistry::type_include(std::string name) {
        auto it = _includes.find(name);
        if (it == _includes.end()) {
            return std::nullopt;
        }
        return it->second;
    }
    std::unique_ptr<CppType> TypeBuilder::cpp_builtin_type(CppBuiltinTypeKind kind) {
        return CppBuiltinType::build(kind);
    }
}  // namespace cppast
