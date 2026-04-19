// Copyright (C) 2017-2023 Jonathan Müller and cppast contributors
// SPDX-License-Identifier: MIT

#include <shark/ast/cpp_forward_declarable.h>

#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_enum.h>
#include <shark/ast/cpp_function.h>
#include <shark/ast/cpp_template.h>
#include <shark/ast/cpp_variable.h>

using namespace cppast;

namespace {
    type_safe::optional_ref<const CppForwardDeclarable> get_declarable(const CppEntity &e) {
        switch (e.kind()) {
            case CppEntityKind::enum_t:
                return type_safe::ref(static_cast<const CppEnum &>(e));
            case CppEntityKind::class_t:
                return type_safe::ref(static_cast<const CppClass &>(e));
            case CppEntityKind::variable_t:
                return type_safe::ref(static_cast<const CppVariable &>(e));
            case CppEntityKind::function_t:
            case CppEntityKind::member_function_t:
            case CppEntityKind::conversion_op_t:
            case CppEntityKind::constructor_t:
            case CppEntityKind::destructor_t:
                return type_safe::ref(static_cast<const CppFunctionBase &>(e));
            case CppEntityKind::function_template_t:
            case CppEntityKind::function_template_specialization_t:
            case CppEntityKind::class_template_t:
            case CppEntityKind::class_template_specialization_t:
                return get_declarable(*static_cast<const CppTemplate &>(e).begin());

            case CppEntityKind::file_t:
            case CppEntityKind::macro_parameter_t:
            case CppEntityKind::macro_definition_t:
            case CppEntityKind::include_directive_t:
            case CppEntityKind::language_linkage_t:
            case CppEntityKind::namespace_t:
            case CppEntityKind::namespace_alias_t:
            case CppEntityKind::using_directive_t:
            case CppEntityKind::using_declaration_t:
            case CppEntityKind::type_alias_t:
            case CppEntityKind::enum_value_t:
            case CppEntityKind::access_specifier_t:
            case CppEntityKind::base_class_t:
            case CppEntityKind::member_variable_t:
            case CppEntityKind::bitfield_t:
            case CppEntityKind::function_parameter_t:
            case CppEntityKind::friend_t:
            case CppEntityKind::template_type_parameter_t:
            case CppEntityKind::non_type_template_parameter_t:
            case CppEntityKind::template_template_parameter_t:
            case CppEntityKind::concept_t:
            case CppEntityKind::alias_template_t:
            case CppEntityKind::variable_template_t:
            case CppEntityKind::static_assert_t:
            case CppEntityKind::unexposed_t:
                return nullptr;

            case CppEntityKind::count:
                break;
        }

        DEBUG_UNREACHABLE(detail::assert_handler{});
        return nullptr;
    }

    type_safe::optional_ref<const CppEntity> get_definition_impl(const CppEntityIndex &idx,
                                                                 const CppEntity &e) {
        auto declarable = get_declarable(e);
        if (!declarable || declarable.value().is_definition())
            // not declarable or is a definition
            // return reference to entity itself
            return type_safe::ref(e);
        // else lookup definition
        return idx.lookup_definition(declarable.value().definition().value());
    }
} // namespace

bool cppast::is_definition(const CppEntity &e) noexcept {
    auto declarable = get_declarable(e);
    return declarable && declarable.value().is_definition();
}

type_safe::optional_ref<const CppEntity> cppast::get_definition(const CppEntityIndex &idx,
                                                                const CppEntity &e) {
    return get_definition_impl(idx, e);
}

type_safe::optional_ref<const CppEnum> cppast::get_definition(const CppEntityIndex &idx,
                                                              const CppEnum &e) {
    return get_definition_impl(idx, e).map([](const CppEntity &e) {
        DEBUG_ASSERT(e.kind() == CppEntityKind::enum_t, detail::assert_handler{});
        return type_safe::ref(static_cast<const CppEnum &>(e));
    });
}

type_safe::optional_ref<const CppClass> cppast::get_definition(const CppEntityIndex &idx,
                                                               const CppClass &e) {
    return get_definition_impl(idx, e).map([](const CppEntity &e) {
        DEBUG_ASSERT(e.kind() == CppEntityKind::class_t, detail::assert_handler{});
        return type_safe::ref(static_cast<const CppClass &>(e));
    });
}

type_safe::optional_ref<const CppVariable> cppast::get_definition(const CppEntityIndex &idx,
                                                                   const CppVariable &e) {
    return get_definition_impl(idx, e).map([](const CppEntity &e) {
        DEBUG_ASSERT(e.kind() == CppEntityKind::variable_t, detail::assert_handler{});
        return type_safe::ref(static_cast<const CppVariable &>(e));
    });
}

type_safe::optional_ref<const CppFunctionBase> cppast::get_definition(const CppEntityIndex &idx,
                                                                        const CppFunctionBase &e) {
    return get_definition_impl(idx, e).map([](const CppEntity &e) {
        DEBUG_ASSERT(is_function(e.kind()), detail::assert_handler{});
        return type_safe::ref(static_cast<const CppFunctionBase &>(e));
    });
}
