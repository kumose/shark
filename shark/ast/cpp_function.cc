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


#include <shark/ast/cpp_function.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppFunctionParameter::kind() noexcept {
    return CppEntityKind::function_parameter_t;
}

std::unique_ptr<CppFunctionParameter> CppFunctionParameter::build(
    const CppEntityIndex &idx, CppEntityId id, std::string name, std::unique_ptr<CppType> type,
    std::unique_ptr<CppExpression> def) {
    auto result = std::unique_ptr<CppFunctionParameter>(
        new CppFunctionParameter(std::move(name), std::move(type), std::move(def)));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

std::unique_ptr<CppFunctionParameter> CppFunctionParameter::build(
    std::unique_ptr<CppType> type, std::unique_ptr<CppExpression> def) {
    return std::unique_ptr<CppFunctionParameter>(
        new CppFunctionParameter("", std::move(type), std::move(def)));
}

CppEntityKind CppFunctionParameter::do_get_entity_kind() const noexcept {
    return kind();
}

std::string CppFunctionBase::do_get_signature() const {
    std::string result = "(";
    for (auto &param: parameters())
        result += to_string(param.type()) + ',';
    if (is_variadic())
        result += "...";

    if (result.back() == ',')
        result.back() = ')';
    else
        result.push_back(')');

    return result;
}

CppEntityKind CppFunction::kind() noexcept {
    return CppEntityKind::function_t;
}

CppEntityKind CppFunction::do_get_entity_kind() const noexcept {
    return kind();
}
