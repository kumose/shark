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


#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

const char *cppast::to_string(CppEntityKind kind) noexcept {
    switch (kind) {
        case CppEntityKind::file_t:
            return "file";

        case CppEntityKind::macro_parameter_t:
            return "macro parameter";
        case CppEntityKind::macro_definition_t:
            return "macro definition";
        case CppEntityKind::include_directive_t:
            return "include directive";

        case CppEntityKind::language_linkage_t:
            return "language linkage";

        case CppEntityKind::namespace_t:
            return "namespace";
        case CppEntityKind::namespace_alias_t:
            return "namespace alias";
        case CppEntityKind::using_directive_t:
            return "using directive";
        case CppEntityKind::using_declaration_t:
            return "using declaration";

        case CppEntityKind::type_alias_t:
            return "type alias";

        case CppEntityKind::enum_t:
            return "enum";
        case CppEntityKind::enum_value_t:
            return "enum value";

        case CppEntityKind::class_t:
            return "class";
        case CppEntityKind::access_specifier_t:
            return "access specifier";
        case CppEntityKind::base_class_t:
            return "base class specifier";

        case CppEntityKind::variable_t:
            return "variable";
        case CppEntityKind::member_variable_t:
            return "member variable";
        case CppEntityKind::bitfield_t:
            return "bit field";

        case CppEntityKind::function_parameter_t:
            return "function parameter";
        case CppEntityKind::function_t:
            return "function";
        case CppEntityKind::member_function_t:
            return "member function";
        case CppEntityKind::conversion_op_t:
            return "conversion operator";
        case CppEntityKind::constructor_t:
            return "constructor";
        case CppEntityKind::destructor_t:
            return "destructor";

        case CppEntityKind::friend_t:
            return "friend";

        case CppEntityKind::template_type_parameter_t:
            return "template type parameter";
        case CppEntityKind::non_type_template_parameter_t:
            return "non type template parameter";
        case CppEntityKind::template_template_parameter_t:
            return "template template parameter";

        case CppEntityKind::alias_template_t:
            return "alias template";
        case CppEntityKind::variable_template_t:
            return "variable template";
        case CppEntityKind::function_template_t:
            return "function template";
        case CppEntityKind::function_template_specialization_t:
            return "function template specialization";
        case CppEntityKind::class_template_t:
            return "class template";
        case CppEntityKind::class_template_specialization_t:
            return "class template specialization";
        case CppEntityKind::concept_t:
            return "concept";

        case CppEntityKind::static_assert_t:
            return "static_assert";

        case CppEntityKind::unexposed_t:
            return "unexposed entity";
        case CppEntityKind::comment_t:
            return "comment";

        case CppEntityKind::count:
            break;
    }

    return "invalid";
}

bool cppast::is_function(CppEntityKind kind) noexcept {
    switch (kind) {
        case CppEntityKind::function_t:
        case CppEntityKind::member_function_t:
        case CppEntityKind::conversion_op_t:
        case CppEntityKind::constructor_t:
        case CppEntityKind::destructor_t:
            return true;

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
        case CppEntityKind::enum_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::class_t:
        case CppEntityKind::access_specifier_t:
        case CppEntityKind::base_class_t:
        case CppEntityKind::variable_t:
        case CppEntityKind::member_variable_t:
        case CppEntityKind::bitfield_t:
        case CppEntityKind::function_parameter_t:
        case CppEntityKind::friend_t:
        case CppEntityKind::template_type_parameter_t:
        case CppEntityKind::non_type_template_parameter_t:
        case CppEntityKind::template_template_parameter_t:
        case CppEntityKind::alias_template_t:
        case CppEntityKind::variable_template_t:
        case CppEntityKind::function_template_t:
        case CppEntityKind::function_template_specialization_t:
        case CppEntityKind::class_template_t:
        case CppEntityKind::class_template_specialization_t:
        case CppEntityKind::concept_t:
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
        case CppEntityKind::comment_t:
        case CppEntityKind::count:
            break;
    }

    return false;
}

bool cppast::is_parameter(CppEntityKind kind) noexcept {
    switch (kind) {
        case CppEntityKind::function_parameter_t:
        case CppEntityKind::template_type_parameter_t:
        case CppEntityKind::non_type_template_parameter_t:
        case CppEntityKind::template_template_parameter_t:
            return true;

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
        case CppEntityKind::enum_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::class_t:
        case CppEntityKind::access_specifier_t:
        case CppEntityKind::base_class_t:
        case CppEntityKind::variable_t:
        case CppEntityKind::member_variable_t:
        case CppEntityKind::bitfield_t:
        case CppEntityKind::function_t:
        case CppEntityKind::member_function_t:
        case CppEntityKind::conversion_op_t:
        case CppEntityKind::constructor_t:
        case CppEntityKind::destructor_t:
        case CppEntityKind::friend_t:
        case CppEntityKind::alias_template_t:
        case CppEntityKind::variable_template_t:
        case CppEntityKind::function_template_t:
        case CppEntityKind::function_template_specialization_t:
        case CppEntityKind::class_template_t:
        case CppEntityKind::class_template_specialization_t:
        case CppEntityKind::concept_t:
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
        case CppEntityKind::comment_t:
        case CppEntityKind::count:
            break;
    }
    return false;
}

bool cppast::is_template(CppEntityKind kind) noexcept {
    switch (kind) {
        case CppEntityKind::alias_template_t:
        case CppEntityKind::variable_template_t:
        case CppEntityKind::function_template_t:
        case CppEntityKind::function_template_specialization_t:
        case CppEntityKind::class_template_t:
        case CppEntityKind::class_template_specialization_t:
        case CppEntityKind::concept_t:
            return true;

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
        case CppEntityKind::enum_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::class_t:
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
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
        case CppEntityKind::comment_t:
        case CppEntityKind::count:
            break;
    }

    return false;
}

bool cppast::is_template_specialization(CppEntityKind kind) noexcept {
    switch (kind) {
        case CppEntityKind::function_template_specialization_t:
        case CppEntityKind::class_template_specialization_t:
            return true;

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
        case CppEntityKind::enum_t:
        case CppEntityKind::enum_value_t:
        case CppEntityKind::class_t:
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
        case CppEntityKind::alias_template_t:
        case CppEntityKind::variable_template_t:
        case CppEntityKind::function_template_t:
        case CppEntityKind::class_template_t:
        case CppEntityKind::concept_t:
        case CppEntityKind::static_assert_t:
        case CppEntityKind::unexposed_t:
        case CppEntityKind::comment_t:
        case CppEntityKind::count:
            break;
    }
    return false;
}
