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


#pragma once

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a type alias/typedef.
    /// \notes There is no distinction between `using` and `typedef` type aliases made in the AST.
    class CppTypeAlias final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered type alias.
        static std::unique_ptr<CppTypeAlias> build(const CppEntityIndex &idx, CppEntityId id,
                                                   std::string name, std::unique_ptr<CppType> type);

        /// \returns A newly created type alias that isn't registered.
        /// \notes This function is intendend for templated type aliases.
        static std::unique_ptr<CppTypeAlias> build(std::string name, std::unique_ptr<CppType> type);

        /// \returns A reference to the aliased [cppast::CppType]().
        const CppType &underlying_type() const noexcept {
            return *type_;
        }

    private:
        CppTypeAlias(std::string name, std::unique_ptr<CppType> type)
            : CppEntity(std::move(name)), type_(std::move(type)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppType> type_;
    };
} // namespace cppast
