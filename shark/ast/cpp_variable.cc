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


#include <shark/ast/cpp_variable.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppVariable::kind() noexcept {
    return CppEntityKind::variable_t;
}

std::unique_ptr<CppVariable> CppVariable::build(
    const CppEntityIndex &idx, CppEntityId id, std::string name, std::unique_ptr<CppType> type,
    std::unique_ptr<CppExpression> def, CppStorageClassSpecifiers spec, bool is_constexpr,
    type_safe::optional<CppEntityRef> semantic_parent) {
    auto result = std::unique_ptr<CppVariable>(
        new CppVariable(std::move(name), std::move(type), std::move(def), spec, is_constexpr));
    result->set_semantic_parent(std::move(semantic_parent));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

std::unique_ptr<CppVariable> CppVariable::build_declaration(
    CppEntityId definition_id, std::string name, std::unique_ptr<CppType> type,
    CppStorageClassSpecifiers spec, bool is_constexpr,
    type_safe::optional<CppEntityRef> semantic_parent) {
    auto result = std::unique_ptr<CppVariable>(
        new CppVariable(std::move(name), std::move(type), nullptr, spec, is_constexpr));
    result->set_semantic_parent(std::move(semantic_parent));
    result->mark_declaration(definition_id);
    return result;
}

CppEntityKind CppVariable::do_get_entity_kind() const noexcept {
    return kind();
}
