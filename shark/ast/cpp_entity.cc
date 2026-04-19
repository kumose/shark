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



#include <shark/ast/cpp_entity.h>

#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_template.h>

using namespace cppast;

CppScopeName::CppScopeName(type_safe::object_ref<const CppEntity> entity) : entity_(entity) {
    if (cppast::is_templated(*entity)) {
        auto &templ = static_cast<const CppTemplate &>(entity->parent().value());
        if (!templ.parameters().empty())
            templ_ = type_safe::ref(templ);
    } else if (is_template(entity->kind())) {
        auto &templ = static_cast<const CppTemplate &>(*entity);
        if (!templ.parameters().empty())
            templ_ = type_safe::ref(templ);
    }
}

const std::string &CppScopeName::name() const noexcept {
    return entity_->name();
}

detail::iteratable_intrusive_list<CppTemplateParameter> CppScopeName::template_parameters()
const noexcept {
    DEBUG_ASSERT(is_templated(), detail::precondition_error_handler{});
    return templ_.value().parameters();
}

CppEntityKind CppUnexposedEntity::kind() noexcept {
    return CppEntityKind::unexposed_t;
}

std::unique_ptr<CppEntity> CppUnexposedEntity::build(const CppEntityIndex &index,
                                                     CppEntityId id, std::string name,
                                                     CppTokenString spelling) {
    std::unique_ptr<CppEntity> result(
        new CppUnexposedEntity(std::move(name), std::move(spelling)));
    index.register_forward_declaration(id, type_safe::ref(*result));
    return result;
}

std::unique_ptr<CppEntity> CppUnexposedEntity::build(CppTokenString spelling) {
    return std::unique_ptr<CppEntity>(new CppUnexposedEntity("", std::move(spelling)));
}

CppEntityKind CppUnexposedEntity::do_get_entity_kind() const noexcept {
    return kind();
}

bool cppast::is_templated(const CppEntity &e) noexcept {
    if (!e.parent())
        return false;
    else if (!is_template(e.parent().value().kind()))
        return false;
    return e.parent().value().name() == e.name();
}
