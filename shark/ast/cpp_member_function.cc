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


#include <shark/ast/cpp_member_function.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

std::string CppMemberFunctionBase::do_get_signature() const {
    auto result = CppFunctionBase::do_get_signature();

    if (is_const(cv_qualifier()))
        result += " const";
    if (is_volatile(cv_qualifier()))
        result += " volatile";

    if (ref_qualifier() == cpp_ref_lvalue)
        result += " &";
    else if (ref_qualifier() == cpp_ref_rvalue)
        result += " &&";

    return result;
}

CppEntityKind CppMemberFunction::kind() noexcept {
    return CppEntityKind::member_function_t;
}

CppEntityKind CppMemberFunction::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppConversionOp::kind() noexcept {
    return CppEntityKind::conversion_op_t;
}

CppEntityKind CppConversionOp::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppConstructor::kind() noexcept {
    return CppEntityKind::constructor_t;
}

CppEntityKind CppConstructor::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppDestructor::kind() noexcept {
    return CppEntityKind::destructor_t;
}

CppEntityKind CppDestructor::do_get_entity_kind() const noexcept {
    return kind();
}
