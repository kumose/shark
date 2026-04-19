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
#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_forward_declarable.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// The keyword used on the declaration of a [cppast::CppClass]().
    enum class CppClassKind {
        class_t,
        struct_t,
        union_t,
    };

    /// \returns The keyword as a string.
    const char *to_string(CppClassKind kind) noexcept;

    /// The C++ access specifiers.
    enum CppAccessSpecifierKind : int {
        cpp_public,
        cpp_protected,
        cpp_private
    };

    /// \returns The access specifier keyword as a string.
    const char *to_string(CppAccessSpecifierKind access) noexcept;

    /// A [cppast::CppEntity]() modelling a C++ access specifier.
    class CppAccessSpecifier final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created access specifier.
        /// \notes It is not meant to be registered at the [cppast::CppEntityIndex](),
        /// as nothing can refer to it.
        static std::unique_ptr<CppAccessSpecifier> build(CppAccessSpecifierKind kind) {
            return std::unique_ptr<CppAccessSpecifier>(new CppAccessSpecifier(kind));
        }

        /// \returns The kind of access specifier.
        CppAccessSpecifierKind access_specifier() const noexcept {
            return access_;
        }

    private:
        CppAccessSpecifier(CppAccessSpecifierKind access)
            : CppEntity(to_string(access)), access_(access) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppAccessSpecifierKind access_;
    };

    /// A [cppast::CppEntity]() modelling a base class specifier.
    class CppBaseClass final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created base class specifier.
        /// \notes It is not meant to be registered at the [cppast::CppEntityIndex](),
        /// as nothing can refer to the specifier itself.
        static std::unique_ptr<CppBaseClass> build(std::string name, std::unique_ptr<CppType> base,
                                                     CppAccessSpecifierKind access, bool is_virtual) {
            return std::unique_ptr<CppBaseClass>(
                new CppBaseClass(std::move(name), std::move(base), access, is_virtual));
        }

        /// \returns The type of the base class.
        const CppType &type() const {
            return *type_;
        }

        /// \returns The access specifier of the base class.
        CppAccessSpecifierKind access_specifier() const noexcept {
            return access_;
        }

        /// \returns Whether or not it is a `virtual` base class.
        bool is_virtual() const noexcept {
            return virtual_;
        }

    private:
        CppBaseClass(std::string name, std::unique_ptr<CppType> base,
                       CppAccessSpecifierKind access, bool is_virtual)
            : CppEntity(std::move(name)), type_(std::move(base)), access_(access), virtual_(is_virtual) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppType> type_;
        CppAccessSpecifierKind access_;
        bool virtual_;
    };

    /// A [cppast::CppEntity]() modelling a C++ class.
    ///
    /// This can either be a definition or just a forward declaration.
    /// If it is just a forward declaration,
    /// everything except the class type will not be available.
    class CppClass final : public CppEntity,
                            public CppEntityContainer<CppClass, CppEntity>,
                            public CppForwardDeclarable {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppClass]().
        class builder {
        public:
            /// \effects Sets the name and kind and whether it is `final`.
            explicit builder(std::string name, CppClassKind kind, bool is_final = false)
                : class_(new CppClass(std::move(name), kind, is_final)) {
            }

            /// \effects Marks the class as final.
            void is_final() noexcept {
                class_->final_ = true;
            }

            /// \effects Builds a [cppast::CppBaseClass]() and adds it.
            CppBaseClass &base_class(std::string name, std::unique_ptr<CppType> type,
                                       CppAccessSpecifierKind access, bool is_virtual) {
                return add_base_class(
                    CppBaseClass::build(std::move(name), std::move(type), access, is_virtual));
            }

            /// \effects Adds a new base class.
            CppBaseClass &add_base_class(std::unique_ptr<CppBaseClass> base) noexcept {
                auto bptr = base.get();
                class_->bases_.push_back(*class_, std::move(base));
                return *bptr;
            }

            /// \effects Builds a [cppast::CppAccessSpecifier]() and adds it.
            void access_specifier(CppAccessSpecifierKind access) {
                add_child(CppAccessSpecifier::build(access));
            }

            /// \effects Adds an entity.
            void add_child(std::unique_ptr<CppEntity> child) noexcept {
                class_->add_child(std::move(child));
            }

            /// \returns The not yet finished class.
            CppClass &get() noexcept {
                return *class_;
            }

            /// \effects Registers the class in the [cppast::CppEntityIndex](),
            /// using the given [cppast::CppEntityId]().
            /// \returns The finished class.
            std::unique_ptr<CppClass> finish(const CppEntityIndex &idx, CppEntityId id,
                                              type_safe::optional<CppEntityRef> semantic_parent);

            /// \effects Marks the class as forward declaration.
            /// \returns The finished class.
            std::unique_ptr<CppClass> finish_declaration(const CppEntityIndex &idx,
                                                          CppEntityId definition_id);

            /// \effects Returns the finished class without registering it.
            /// \notes This is intended for templated classes only.
            std::unique_ptr<CppClass> finish(type_safe::optional<CppEntityRef> semantic_parent);

            /// \effects Returns the finish class without registering it and marks it as forward
            /// declaration. \notes This is intended for templated classes only.
            std::unique_ptr<CppClass> finish_declaration(CppEntityId definition_id);

        private:
            std::unique_ptr<CppClass> class_;
        };

        /// \returns The keyword used in the declaration of the class.
        CppClassKind class_kind() const noexcept {
            return kind_;
        }

        /// \returns Whether or not the class was declared `final`.
        bool is_final() const noexcept {
            return final_;
        }

        /// \returns An iteratable object iterating over the [cppast::CppBaseClass]() specifiers.
        detail::iteratable_intrusive_list<CppBaseClass> bases() const noexcept {
            return type_safe::ref(bases_);
        }

    private:
        CppClass(std::string name, CppClassKind kind, bool final)
            : CppEntity(std::move(name)), kind_(kind), final_(final) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        type_safe::optional<CppScopeName> do_get_scope_name() const override {
            return type_safe::ref(*this);
        }

        detail::intrusive_list<CppBaseClass> bases_;
        CppClassKind kind_;
        bool final_;
    };

    /// \returns The type the base class refers to.
    /// It is either a class or some form of typedef.
    type_safe::optional_ref<const CppEntity> get_class_or_typedef(const CppEntityIndex &index,
                                                                  const CppBaseClass &base);

    /// \returns The type the base class refers to.
    /// Typedefs are unwrapped.
    type_safe::optional_ref<const CppClass> get_class(const CppEntityIndex &index,
                                                       const CppBaseClass &base);
} // namespace cppast
