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

#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_entity_ref.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a namespace.
    class CppNamespace final : public CppEntity,
                               public CppEntityContainer<CppNamespace, CppEntity> {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppNamespace]().
        class builder {
        public:
            /// \effects Sets the namespace name and whether it is inline and nested.
            explicit builder(std::string name, bool is_inline, bool is_nested)
                : namespace_(new CppNamespace(std::move(name), is_inline, is_nested)) {
            }

            /// \effects Adds an entity.
            void add_child(std::unique_ptr<CppEntity> child) noexcept {
                namespace_->add_child(std::move(child));
            }

            /// \returns The not yet finished namespace.
            CppNamespace &get() const noexcept {
                return *namespace_;
            }

            /// \effects Registers the namespace in the [cppast::CppEntityIndex](),
            /// using the given [cppast::CppEntityId]().
            /// \returns The finished namespace.
            std::unique_ptr<CppNamespace> finish(const CppEntityIndex &idx, CppEntityId id) {
                idx.register_namespace(std::move(id), type_safe::ref(*namespace_));
                return std::move(namespace_);
            }

        private:
            std::unique_ptr<CppNamespace> namespace_;
        };

        /// \returns Whether or not the namespace is an `inline namespace`.
        bool is_inline() const noexcept {
            return inline_;
        }

        /// \returns Whether or not the namespace is part of a C++17 nested namespace.
        bool is_nested() const noexcept {
            return nested_;
        }

        /// \returns Whether or not the namespace is anonymous.
        bool is_anonymous() const noexcept {
            return name().empty();
        }

    private:
        CppNamespace(std::string name, bool is_inline, bool is_nested)
            : CppEntity(std::move(name)), inline_(is_inline), nested_(is_nested) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        type_safe::optional<CppScopeName> do_get_scope_name() const override {
            return type_safe::ref(*this);
        }

        bool inline_;
        bool nested_;
    };

    /// \exclude
    namespace detail {
        struct CppNamespaceRefPredicate {
            bool operator()(const CppEntity &e);
        };
    } // namespace detail

    /// A reference to a [cppast::CppNamespace]().
    using CppNamespaceRef = BasicCppEntityRef<CppNamespace, detail::CppNamespaceRefPredicate>;

    /// A [cppast::CppEntity]() modelling a namespace alias.
    class CppNamespaceAlias final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered namespace alias.
        static std::unique_ptr<CppNamespaceAlias> build(const CppEntityIndex &idx, CppEntityId id,
                                                        std::string name, CppNamespaceRef target);

        /// \returns The [cppast::CppNamespaceRef]() to the aliased namespace.
        /// \notes If the namespace aliases aliases another namespace alias,
        /// the target entity will still be the namespace, not the alias.
        const CppNamespaceRef &target() const noexcept {
            return target_;
        }

    private:
        CppNamespaceAlias(std::string name, CppNamespaceRef target)
            : CppEntity(std::move(name)), target_(std::move(target)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppNamespaceRef target_;
    };

    /// A [cppast::CppEntity]() modelling a using directive.
    ///
    /// A using directive is `using namespace std`, for example.
    /// \notes It does not have a name.
    class CppUsingDirective final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created using directive.
        /// \notes It is not meant to be registered at the [cppast::CppEntityIndex](),
        /// as nothing can refer to it.
        static std::unique_ptr<CppUsingDirective> build(CppNamespaceRef target) {
            return std::unique_ptr<CppUsingDirective>(new CppUsingDirective(std::move(target)));
        }

        /// \returns The [cppast::CppNamespaceRef]() that is being used.
        const CppNamespaceRef &target() const {
            return target_;
        }

    private:
        CppUsingDirective(CppNamespaceRef target) : CppEntity(""), target_(std::move(target)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppNamespaceRef target_;
    };

    /// A [cppast::CppEntity]() modelling a using declaration.
    ///
    /// A using declaration is `using std::vector`, for example.
    /// \notes It does not have a name.
    class CppUsingDeclaration final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created using declaration.
        /// \notes It is not meant to be registered at the [cppast::CppEntityIndex](),
        /// as nothing can refer to it.
        static std::unique_ptr<CppUsingDeclaration> build(CppEntityRef target) {
            return std::unique_ptr<CppUsingDeclaration>(new CppUsingDeclaration(std::move(target)));
        }

        /// \returns The [cppast::CppEntityRef]() that is being used.
        /// \notes The name of the reference is the same as the name of this entity.
        const CppEntityRef &target() const noexcept {
            return target_;
        }

    private:
        CppUsingDeclaration(CppEntityRef target) : CppEntity(""), target_(std::move(target)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppEntityRef target_;
    };
} // namespace cppast
