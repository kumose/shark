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

#include <type_traits>

#include <typesafe/optional.hpp>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_ref.h>

namespace cppast {
    /// Mixin base class for all entities that can have a forward declaration.
    ///
    /// Examples are [cppast::CppEnum]() or [cppast::CppClass](),
    /// but also [cppast::CppFunctionBase]().
    /// Those entities can have multiple declarations and one definition.
    class CppForwardDeclarable {
    public:
        /// \returns Whether or not the entity is the definition.
        bool is_definition() const noexcept {
            return !definition_.has_value();
        }

        /// \returns Whether or not the entity is "just" a declaration.
        bool is_declaration() const noexcept {
            return definition_.has_value();
        }

        /// \returns The [cppast::CppEntityId]() of the definition,
        /// if the current entity is not the definition.
        const type_safe::optional<CppEntityId> &definition() const noexcept {
            return definition_;
        }

        /// \returns A reference to the semantic parent of the entity.
        /// This applies only to out-of-line definitions
        /// and is the entity which owns the declaration.
        const type_safe::optional<CppEntityRef> &semantic_parent() const noexcept {
            return semantic_parent_;
        }

        /// \returns The name of the semantic parent, if it has one,
        /// else the empty string.
        /// \notes This may include template parameters.
        std::string semantic_scope() const noexcept {
            return type_safe::copy(semantic_parent_.map(&CppEntityRef::name)).value_or("");
        }

    protected:
        /// \effects Marks the entity as definition.
        /// \notes If it is not a definition,
        /// [*set_definition]() must be called.
        CppForwardDeclarable() noexcept = default;

        ~CppForwardDeclarable() noexcept = default;

        /// \effects Sets the definition entity,
        /// marking it as a forward declaration.
        void mark_declaration(CppEntityId def) noexcept {
            definition_ = std::move(def);
        }

        /// \effects Sets the semantic parent of the entity.
        void set_semantic_parent(type_safe::optional<CppEntityRef> semantic_parent) noexcept {
            semantic_parent_ = std::move(semantic_parent);
        }

    private:
        type_safe::optional<CppEntityRef> semantic_parent_;
        type_safe::optional<CppEntityId> definition_;
    };

    /// \returns Whether or not the given entity is a definition.
    bool is_definition(const CppEntity &e) noexcept;

    /// Gets the definition of an entity.
    /// \returns A [ts::optional_ref]() to the entity that is the definition.
    /// If the entity is a definition or not derived from [cppast::CppForwardDeclarable]() (only valid
    /// for the generic entity overload), returns a reference to the entity itself. Otherwise lookups
    /// the definition id and returns it. \notes The return value will only be `nullptr`, if the
    /// definition is not registered. \group get_definition
    type_safe::optional_ref<const CppEntity> get_definition(const CppEntityIndex &idx,
                                                            const CppEntity &e);

    /// \group get_definition
    type_safe::optional_ref<const CppEnum> get_definition(const CppEntityIndex &idx,
                                                          const CppEnum &e);

    /// \group get_definition
    type_safe::optional_ref<const CppClass> get_definition(const CppEntityIndex &idx,
                                                           const CppClass &e);

    /// \group get_definition
    type_safe::optional_ref<const CppVariable> get_definition(const CppEntityIndex &idx,
                                                               const CppVariable &e);

    /// \group get_definition
    type_safe::optional_ref<const CppFunctionBase> get_definition(const CppEntityIndex &idx,
                                                                    const CppFunctionBase &e);
} // namespace cppast
