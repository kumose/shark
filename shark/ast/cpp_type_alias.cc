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


#include <shark/ast/cpp_type_alias.h>

#include <shark/ast/cpp_entity_kind.h>

using namespace cppast;

CppEntityKind CppTypeAlias::kind() noexcept
{
    return CppEntityKind::type_alias_t;
}

std::unique_ptr<CppTypeAlias> CppTypeAlias::build(const CppEntityIndex& idx, CppEntityId id,
                                                      std::string               name,
                                                      std::unique_ptr<CppType> type)
{
    auto result = build(std::move(name), std::move(type));
    idx.register_forward_declaration(std::move(id), type_safe::cref(*result)); // not a definition
    return result;
}

std::unique_ptr<CppTypeAlias> CppTypeAlias::build(std::string               name,
                                                      std::unique_ptr<CppType> type)
{
    return std::unique_ptr<CppTypeAlias>(new CppTypeAlias(std::move(name), std::move(type)));
}

CppEntityKind CppTypeAlias::do_get_entity_kind() const noexcept
{
    return kind();
}
