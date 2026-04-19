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


#include <shark/ast/cpp_template_parameter.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

const char *cppast::to_string(cpp_template_keyword kw) noexcept {
    switch (kw) {
        case cpp_template_keyword::keyword_class:
            return "class";
        case cpp_template_keyword::keyword_typename:
            return "typename";
        case cpp_template_keyword::concept_contraint:
            return "concept constraint data lost";
    }

    return "should not get here";
}

std::unique_ptr<CppTemplateTypeParameter> CppTemplateTypeParameter::build(
    const CppEntityIndex &idx, CppEntityId id, std::string name, cpp_template_keyword kw,
    bool variadic, std::unique_ptr<CppType> default_type,
    type_safe::optional<CppTokenString> concept_constraint) {
    std::unique_ptr<CppTemplateTypeParameter> result(
        new CppTemplateTypeParameter(std::move(name), kw, variadic, std::move(default_type),
                                     std::move(concept_constraint)));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

CppEntityKind CppTemplateTypeParameter::kind() noexcept {
    return CppEntityKind::template_type_parameter_t;
}

CppEntityKind CppTemplateTypeParameter::do_get_entity_kind() const noexcept {
    return kind();
}

bool detail::cpp_template_parameter_ref_predicate::operator()(const CppEntity &e) {
    return e.kind() == CppEntityKind::template_type_parameter_t;
}

std::unique_ptr<CppNonTypeTemplateParameter> CppNonTypeTemplateParameter::build(
    const CppEntityIndex &idx, CppEntityId id, std::string name, std::unique_ptr<CppType> type,
    bool is_variadic, std::unique_ptr<CppExpression> default_value) {
    std::unique_ptr<CppNonTypeTemplateParameter> result(
        new CppNonTypeTemplateParameter(std::move(name), std::move(type), is_variadic,
                                        std::move(default_value)));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

CppEntityKind CppNonTypeTemplateParameter::kind() noexcept {
    return CppEntityKind::non_type_template_parameter_t;
}

CppEntityKind CppNonTypeTemplateParameter::do_get_entity_kind() const noexcept {
    return kind();
}

bool detail::CppTemplateRefPredicate::operator()(const CppEntity &e) {
    return is_template(e.kind()) || e.kind() == CppEntityKind::template_template_parameter_t;
}

CppEntityKind CppTemplateTemplateParameter::kind() noexcept {
    return CppEntityKind::template_template_parameter_t;
}

CppEntityKind CppTemplateTemplateParameter::do_get_entity_kind() const noexcept {
    return kind();
}
