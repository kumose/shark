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


#include <memory>

#include <typesafe/optional_ref.hpp>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_expression.h>
#include <shark/ast/cpp_forward_declarable.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling the value of an [cppast::CppEnum]().
    class CppEnumValue final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered enum value.
        /// \notes `value` may be `nullptr`, in which case the enum has an implicit value.
        static std::unique_ptr<CppEnumValue> build(const CppEntityIndex &idx, CppEntityId id,
                                                     std::string name,
                                                     std::unique_ptr<CppExpression> value = nullptr);

        /// \returns A [ts::optional_ref]() to the [cppast::CppExpression]() that is the enum value.
        /// \notes It only has an associated expression if the value is explictly given.
        type_safe::optional_ref<const CppExpression> value() const noexcept {
            return type_safe::opt_cref(value_.get());
        }

    private:
        CppEnumValue(std::string name, std::unique_ptr<CppExpression> value)
            : CppEntity(std::move(name)), value_(std::move(value)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppExpression> value_;
    };

    /// A [cppast::CppEntity]() modelling a C++ enumeration.
    ///
    /// This can either be a definition or just a forward declaration.
    /// If it is just forward declared, it will not have any children.
    class CppEnum final : public CppEntity,
                           public CppEntityContainer<CppEnum, CppEnumValue>,
                           public CppForwardDeclarable {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppEnum]().
        class builder {
        public:
            /// \effects Sets the name, underlying type and whether it is scoped.
            builder(std::string name, bool scoped, std::unique_ptr<CppType> type, bool explicit_type)
                : enum_(new CppEnum(std::move(name), std::move(type), explicit_type, scoped)) {
            }

            /// \effects Adds a [cppast::CppEnumValue]().
            void add_value(std::unique_ptr<CppEnumValue> value) {
                enum_->add_child(std::move(value));
            }

            /// \returns The not yet finished enumeration.
            CppEnum &get() noexcept {
                return *enum_;
            }

            /// \effects Registers the enum in the [cppast::CppEntityIndex](),
            /// using the given [cppast::CppEntityId]().
            /// \returns The finished enum.
            std::unique_ptr<CppEnum> finish(
                const CppEntityIndex &idx, CppEntityId id,
                type_safe::optional<CppEntityRef> semantic_parent) noexcept {
                enum_->set_semantic_parent(std::move(semantic_parent));
                idx.register_definition(std::move(id), type_safe::ref(*enum_));
                return std::move(enum_);
            }

            /// \effects Marks the enum as forward declaration.
            /// \returns The finished enum.
            std::unique_ptr<CppEnum> finish_declaration(const CppEntityIndex &idx,
                                                         CppEntityId definition_id) noexcept {
                enum_->mark_declaration(definition_id);
                idx.register_forward_declaration(std::move(definition_id), type_safe::ref(*enum_));
                return std::move(enum_);
            }

        private:
            std::unique_ptr<CppEnum> enum_;
        };

        /// \returns A reference to the underlying [cppast::CppType]() of the enum.
        const CppType &underlying_type() const noexcept {
            return *type_;
        }

        /// \returns Whether or not the underlying type is explictly given.
        bool has_explicit_type() const noexcept {
            return type_given_;
        }

        /// \returns Whether or not it is a scoped enumeration (i.e. an `enum class`).
        bool is_scoped() const noexcept {
            return scoped_;
        }

    private:
        CppEnum(std::string name, std::unique_ptr<CppType> type, bool type_given, bool scoped)
            : CppEntity(std::move(name)), type_(std::move(type)), scoped_(scoped), type_given_(type_given) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        type_safe::optional<CppScopeName> do_get_scope_name() const override;

        std::unique_ptr<CppType> type_;
        bool scoped_, type_given_;
    };
} // namespace cppast
