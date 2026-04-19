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


#include <shark/ast/cpp_enum.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppEnumValue::kind() noexcept {
    return CppEntityKind::enum_value_t;
}

std::unique_ptr<CppEnumValue> CppEnumValue::build(const CppEntityIndex &idx, CppEntityId id,
                                                  std::string name,
                                                  std::unique_ptr<CppExpression> value) {
    auto result
            = std::unique_ptr<CppEnumValue>(new CppEnumValue(std::move(name), std::move(value)));
    idx.register_definition(std::move(id), type_safe::ref(*result));
    return result;
}

CppEntityKind CppEnumValue::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppEnum::kind() noexcept {
    return CppEntityKind::enum_t;
}

CppEntityKind CppEnum::do_get_entity_kind() const noexcept {
    return kind();
}

type_safe::optional<CppScopeName> CppEnum::do_get_scope_name() const {
    if (scoped_)
        return type_safe::ref(*this);
    return type_safe::nullopt;
}
