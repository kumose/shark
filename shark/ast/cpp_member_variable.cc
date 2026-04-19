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



#include <shark/ast/cpp_member_variable.h>
#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppMemberVariable::kind() noexcept {
    return CppEntityKind::member_variable_t;
}

std::unique_ptr<CppMemberVariable> CppMemberVariable::build(const CppEntityIndex &idx,
                                                            CppEntityId id, std::string name,
                                                            std::unique_ptr<CppType> type,
                                                            std::unique_ptr<CppExpression> def,
                                                            bool is_mutable) {
    auto result = std::unique_ptr<CppMemberVariable>(
        new CppMemberVariable(std::move(name), std::move(type), std::move(def), is_mutable));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

CppEntityKind CppMemberVariable::do_get_entity_kind() const noexcept {
    return kind();
}

CppEntityKind CppBitfield::kind() noexcept {
    return CppEntityKind::bitfield_t;
}

std::unique_ptr<CppBitfield> CppBitfield::build(const CppEntityIndex &idx, CppEntityId id,
                                                std::string name, std::unique_ptr<CppType> type,
                                                unsigned no_bits, bool is_mutable) {
    auto result = std::unique_ptr<CppBitfield>(
        new CppBitfield(std::move(name), std::move(type), no_bits, is_mutable));
    idx.register_definition(std::move(id), type_safe::cref(*result));
    return result;
}

std::unique_ptr<CppBitfield> CppBitfield::build(std::unique_ptr<CppType> type, unsigned no_bits,
                                                bool is_mutable) {
    return std::unique_ptr<CppBitfield>(
        new CppBitfield("", std::move(type), no_bits, is_mutable));
}

CppEntityKind CppBitfield::do_get_entity_kind() const noexcept {
    return kind();
}
