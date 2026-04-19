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
#include <shark/ast/cpp_forward_declarable.h>
#include <shark/ast/cpp_storage_class_specifiers.h>
#include <shark/ast/cpp_variable_base.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a C++ variable.
    /// \notes This is not a member variable,
    /// use [cppast::CppMemberVariable]() for that.
    /// But it can be `static` member variable.
    class CppVariable final : public CppEntity,
                              public CppVariableBase,
                              public CppForwardDeclarable {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered variable.
        /// \notes The default value may be `nullptr` indicating no default value.
        static std::unique_ptr<CppVariable> build(const CppEntityIndex &idx, CppEntityId id,
                                                  std::string name, std::unique_ptr<CppType> type,
                                                  std::unique_ptr<CppExpression> def,
                                                  CppStorageClassSpecifiers spec, bool is_constexpr,
                                                  type_safe::optional<CppEntityRef> semantic_parent
                                                          = {});

        /// \returns A newly created variable that is a declaration.
        /// A declaration will not be registered and it does not have the default value.
        static std::unique_ptr<CppVariable> build_declaration(
            CppEntityId definition_id, std::string name, std::unique_ptr<CppType> type,
            CppStorageClassSpecifiers spec, bool is_constexpr,
            type_safe::optional<CppEntityRef> semantic_parent = {});

        /// \returns The [cppast::cpp_storage_specifiers]() on that variable.
        CppStorageClassSpecifiers storage_class() const noexcept {
            return storage_;
        }

        /// \returns Whether the variable is marked `constexpr`.
        bool is_constexpr() const noexcept {
            return is_constexpr_;
        }

    private:
        CppVariable(std::string name, std::unique_ptr<CppType> type,
                    std::unique_ptr<CppExpression> def, CppStorageClassSpecifiers spec,
                    bool is_constexpr)
            : CppEntity(std::move(name)), CppVariableBase(std::move(type), std::move(def)),
              storage_(spec), is_constexpr_(is_constexpr) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppStorageClassSpecifiers storage_;
        bool is_constexpr_;
    };
} // namespace cppast
