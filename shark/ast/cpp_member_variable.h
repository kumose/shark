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
#include <shark/ast/cpp_variable_base.h>

namespace cppast {
    /// Base class for all kinds of member variables.
    class CppMemberVariableBase : public CppEntity, public CppVariableBase {
    public:
        /// \returns Whether or not the member variable is declared `mutable`.
        bool is_mutable() const noexcept {
            return mutable_;
        }

        CppMemberVariableBase(std::string name, std::unique_ptr<CppType> type,
                              std::unique_ptr<CppExpression> def, bool is_mutable)
            : CppEntity(std::move(name)), CppVariableBase(std::move(type), std::move(def)),
              mutable_(is_mutable) {
        }

    private:
        bool mutable_;
    };

    /// A [cppast::CppEntity]() modelling a C++ member variable.
    class CppMemberVariable final : public CppMemberVariableBase {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered member variable.
        /// \notes `def` may be `nullptr` in which case there is no member initializer provided.
        static std::unique_ptr<CppMemberVariable> build(const CppEntityIndex &idx, CppEntityId id,
                                                        std::string name,
                                                        std::unique_ptr<CppType> type,
                                                        std::unique_ptr<CppExpression> def,
                                                        bool is_mutable);

    private:
        using CppMemberVariableBase::CppMemberVariableBase;

        CppEntityKind do_get_entity_kind() const noexcept override;
    };

    /// A [cppast::CppEntity]() modelling a C++ bitfield.
    class CppBitfield final : public CppMemberVariableBase {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered bitfield.
        /// \notes It cannot have a member initializer, i.e. default value.
        static std::unique_ptr<CppBitfield> build(const CppEntityIndex &idx, CppEntityId id,
                                                  std::string name, std::unique_ptr<CppType> type,
                                                  unsigned no_bits, bool is_mutable);

        /// \returns A newly created unnamed bitfield.
        /// \notes It will not be registered, as it is unnamed.
        static std::unique_ptr<CppBitfield> build(std::unique_ptr<CppType> type, unsigned no_bits,
                                                  bool is_mutable);

        /// \returns The number of bits of the bitfield.
        unsigned no_bits() const noexcept {
            return bits_;
        }

    private:
        CppBitfield(std::string name, std::unique_ptr<CppType> type, unsigned no_bits,
                    bool is_mutable)
            : CppMemberVariableBase(std::move(name), std::move(type), nullptr, is_mutable),
              bits_(no_bits) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        unsigned bits_;
    };
} // namespace cppast
