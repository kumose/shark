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


#include <shark/ast/cpp_namespace.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppNamespace::kind() noexcept {
    return CppEntityKind::namespace_t;
}

CppEntityKind CppNamespace::do_get_entity_kind() const noexcept {
    return kind();
}

bool detail::CppNamespaceRefPredicate::operator()(const CppEntity &e) {
    return e.kind() == CppEntityKind::namespace_t;
}

std::unique_ptr<CppNamespaceAlias> CppNamespaceAlias::build(const CppEntityIndex &idx,
                                                            CppEntityId id, std::string name,
                                                            CppNamespaceRef target) {
    auto ptr = std::unique_ptr<CppNamespaceAlias>(
        new CppNamespaceAlias(std::move(name), std::move(target)));
    idx.register_forward_declaration(std::move(id), type_safe::ref(*ptr));
    return ptr;
}

CppEntityKind CppNamespaceAlias::kind() noexcept {
    return CppEntityKind::namespace_alias_t;
}

CppEntityKind CppNamespaceAlias::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppUsingDirective::kind() noexcept {
    return CppEntityKind::using_directive_t;
}

CppEntityKind CppUsingDirective::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppUsingDeclaration::kind() noexcept {
    return CppEntityKind::using_declaration_t;
}

CppEntityKind CppUsingDeclaration::do_get_entity_kind() const noexcept {
    return kind();
}
