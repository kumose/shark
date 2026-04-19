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


#include <shark/ast/visitor.h>
#include <shark/ast/cpp_alias_template.h>
#include <shark/ast/cpp_class.h>
#include <shark/ast/cpp_class_template.h>
#include <shark/ast/cpp_enum.h>
#include <shark/ast/cpp_file.h>
#include <shark/ast/cpp_function_template.h>
#include <shark/ast/cpp_language_linkage.h>
#include <shark/ast/cpp_namespace.h>
#include <shark/ast/cpp_variable_template.h>

using namespace cppast;

namespace {
    CppAccessSpecifierKind get_initial_access(const CppEntity &e) {
        if (e.kind() == CppClass::kind())
            return static_cast<const CppClass &>(e).class_kind() == CppClassKind::class_t
                       ? cpp_private
                       : cpp_public;
        return cpp_public;
    }

    void update_access(CppAccessSpecifierKind &child_access, const CppEntity &child) {
        if (child.kind() == CppAccessSpecifier::kind())
            child_access = static_cast<const CppAccessSpecifier &>(child).access_specifier();
    }

    template<typename T>
    bool handle_container(const CppEntity &e, detail::visitor_callback_t cb, void *functor,
                          CppAccessSpecifierKind cur_access, bool last_child) {
        auto &container = static_cast<const T &>(e);

        auto handle_children
                = cb(functor, container, {VisitorInfo::container_entity_enter, cur_access, last_child});
        if (handle_children) {
            auto child_access = get_initial_access(e);
            for (auto iter = container.begin(); iter != container.end();) {
                auto &cur = *iter;
                ++iter;

                update_access(child_access, cur);

                if (!detail::visit(cur, cb, functor, child_access, iter == container.end()))
                    return false;
            }
        }

        return cb(functor, container, {VisitorInfo::container_entity_exit, cur_access, last_child});
    }
} // namespace

bool detail::visit(const CppEntity &e, detail::visitor_callback_t cb, void *functor,
                   CppAccessSpecifierKind cur_access, bool last_child) {
    switch (e.kind()) {
        case CppEntityKind::file_t:
            return handle_container<CppFile>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::language_linkage_t:
            return handle_container<CppLanguageLinkage>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::namespace_t:
            return handle_container<CppNamespace>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::enum_t:
            return handle_container<CppEnum>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::class_t:
            return handle_container<CppClass>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::alias_template_t:
            return handle_container<CppAliasTemplate>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::variable_template_t:
            return handle_container<CppVariableTemplate>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::function_template_t:
            return handle_container<CppFunctionTemplate>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::function_template_specialization_t:
            return handle_container<CppFunctionTemplateSpecialization>(e, cb, functor, cur_access,
                                                                       last_child);
        case CppEntityKind::class_template_t:
            return handle_container<CppClassTemplate>(e, cb, functor, cur_access, last_child);
        case CppEntityKind::class_template_specialization_t:
            return handle_container<CppClassTemplateSpecialization>(e, cb, functor, cur_access,
                                                                    last_child);

        case CppEntityKind::macro_parameter_t:
        case CppEntityKind::macro_definition_t:
        case CppEntityKind::include_directive_t:
        case CppEntityKind::namespace_alias_t:
        case CppEntityKind::using_directive_t:
        case CppEntityKind::using_declaration_t:
        case CppEntityKind::type_alias_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::access_specifier_t:
        case CppEntityKind::base_class_t:
        case CppEntityKind::variable_t:
        case CppEntityKind::member_variable_t:
        case CppEntityKind::bitfield_t:
        case CppEntityKind::function_parameter_t:
        case CppEntityKind::function_t:
        case CppEntityKind::member_function_t:
        case CppEntityKind::conversion_op_t:
        case CppEntityKind::constructor_t:
        case CppEntityKind::destructor_t:
        case CppEntityKind::friend_t:
        case CppEntityKind::template_type_parameter_t:
        case CppEntityKind::non_type_template_parameter_t:
        case CppEntityKind::template_template_parameter_t:
        case CppEntityKind::concept_t:
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
            return cb(functor, e, {VisitorInfo::leaf_entity, cur_access, last_child});

        case CppEntityKind::count:
            break;
    }

    DEBUG_UNREACHABLE(detail::assert_handler{});
    return true;
}
