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


#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_alias_template.h>
#include <shark/ast/cpp_class_template.h>
#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

std::unique_ptr<CppClass> CppClass::builder::finish(
    const CppEntityIndex &idx, CppEntityId id,
    type_safe::optional<CppEntityRef> semantic_parent) {
    class_->set_semantic_parent(std::move(semantic_parent));
    idx.register_definition(std::move(id), type_safe::ref(*class_));
    return std::move(class_);
}

std::unique_ptr<CppClass> CppClass::builder::finish_declaration(const CppEntityIndex &idx,
                                                                CppEntityId definition_id) {
    class_->mark_declaration(definition_id);
    idx.register_forward_declaration(std::move(definition_id), type_safe::ref(*class_));
    return std::move(class_);
}

std::unique_ptr<CppClass> CppClass::builder::finish(
    type_safe::optional<CppEntityRef> semantic_parent) {
    class_->set_semantic_parent(std::move(semantic_parent));
    return std::move(class_);
}

std::unique_ptr<CppClass> CppClass::builder::finish_declaration(CppEntityId definition_id) {
    class_->mark_declaration(definition_id);
    return std::move(class_);
}

const char *cppast::to_string(CppClassKind kind) noexcept {
    switch (kind) {
        case CppClassKind::class_t:
            return "class";
        case CppClassKind::struct_t:
            return "struct";
        case CppClassKind::union_t:
            return "union";
    }

    return "should not get here";
}

const char *cppast::to_string(CppAccessSpecifierKind access) noexcept {
    switch (access) {
        case cpp_public:
            return "public";
        case cpp_protected:
            return "protected";
        case cpp_private:
            return "private";
    }

    return "should not get here either";
}

CppEntityKind CppAccessSpecifier::kind() noexcept {
    return CppEntityKind::access_specifier_t;
}

CppEntityKind CppAccessSpecifier::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppBaseClass::kind() noexcept {
    return CppEntityKind::base_class_t;
}

CppEntityKind CppBaseClass::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppClass::kind() noexcept {
    return CppEntityKind::class_t;
}

CppEntityKind CppClass::do_get_entity_kind() const noexcept {
    return kind();
}

namespace {
    CppEntityRef get_type_ref(const CppType &type) {
        if (type.kind() == CppTypeKind::user_defined_t) {
            auto &ref = static_cast<const CppUserDefinedType &>(type).entity();
            return CppEntityRef(ref.id()[0u], ref.name());
        } else if (type.kind() == CppTypeKind::template_instantiation_t) {
            auto &ref = static_cast<const CppTemplateInstantiationType &>(type).primary_template();
            return CppEntityRef(ref.id()[0u], ref.name());
        }

        DEBUG_ASSERT(type.kind() == CppTypeKind::template_parameter_t
                     || type.kind() == CppTypeKind::decltype_t
                     || type.kind() == CppTypeKind::decltype_auto_t
                     || type.kind() == CppTypeKind::unexposed_t,
                     detail::assert_handler{});
        return CppEntityRef(CppEntityId("<null id>"), "");
    }

    type_safe::optional_ref<const CppEntity> get_entity_impl(const CppEntityIndex &index,
                                                             const CppEntityRef &ref) {
        auto result = ref.get(index);
        if (result.empty())
            return nullptr;
        DEBUG_ASSERT(result.size() == 1u, detail::assert_handler{});

        auto entity = result.front();
        if (entity->kind() == CppClassTemplate::kind())
            return type_safe::ref(static_cast<const CppClassTemplate &>(*entity).class_());
        else if (entity->kind() == CppClassTemplateSpecialization::kind())
            return type_safe::ref(
                static_cast<const CppClassTemplateSpecialization &>(*entity).class_());
        else
            return entity;
    }

    type_safe::optional_ref<const CppClass> get_class_impl(const CppEntityIndex &index,
                                                           const CppEntityRef &ref) {
        auto entity = get_entity_impl(index, ref);
        if (!entity)
            return nullptr;

        if (entity.value().kind() == CppAliasTemplate::kind()) {
            auto &alias = static_cast<const cppast::CppAliasTemplate &>(entity.value());
            return get_class_impl(index, get_type_ref(alias.type_alias().underlying_type()));
        } else if (entity.value().kind() == CppTypeAlias::kind()) {
            auto &alias = static_cast<const cppast::CppTypeAlias &>(entity.value());
            return get_class_impl(index, get_type_ref(alias.underlying_type()));
        } else {
            DEBUG_ASSERT(entity.value().kind() == CppClass::kind(), detail::assert_handler{});
            return type_safe::ref(static_cast<const CppClass &>(entity.value()));
        }
    }
} // namespace

type_safe::optional_ref<const CppClass> cppast::get_class(const CppEntityIndex &index,
                                                          const CppBaseClass &base) {
    return get_class_impl(index, get_type_ref(base.type()));
}

type_safe::optional_ref<const CppEntity> cppast::get_class_or_typedef(
    const CppEntityIndex &index, const CppBaseClass &base) {
    return get_entity_impl(index, get_type_ref(base.type()));
}
