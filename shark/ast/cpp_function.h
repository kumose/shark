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
#include <shark/detail/intrusive_list.hpp>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a function parameter.
    class CppFunctionParameter final : public CppEntity, public CppVariableBase {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered function parameter.
        static std::unique_ptr<CppFunctionParameter> build(const CppEntityIndex &idx,
                                                           CppEntityId id, std::string name,
                                                           std::unique_ptr<CppType> type,
                                                           std::unique_ptr<CppExpression> def
                                                                   = nullptr);

        /// \returns A newly created unnamed function parameter.
        /// \notes It will not be registered, as nothing can refer to it.
        static std::unique_ptr<CppFunctionParameter> build(std::unique_ptr<CppType> type,
                                                           std::unique_ptr<CppExpression> def
                                                                   = nullptr);

    private:
        CppFunctionParameter(std::string name, std::unique_ptr<CppType> type,
                             std::unique_ptr<CppExpression> def)
            : CppEntity(std::move(name)), CppVariableBase(std::move(type), std::move(def)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;
    };

    /// The kinds of function bodies of a [cppast::CppFunctionBase]().
    enum CppFunctionBodyKind : int {
        cpp_function_declaration, //< Just a declaration.
        cpp_function_definition, //< Regular definition.
        cpp_function_defaulted, //< Defaulted definition.
        cpp_function_deleted, //< Deleted definition.
    };

    /// \returns Whether or not the function body is a declaration,
    /// without a definition.
    inline bool is_declaration(CppFunctionBodyKind body) noexcept {
        return body == cpp_function_declaration;
    }

    /// \returns Whether or not the function body is a definition.
    inline bool is_definition(CppFunctionBodyKind body) noexcept {
        return !is_declaration(body);
    }

    /// Base class for all entities that are functions.
    ///
    /// It contains arguments and common flags.
    class CppFunctionBase : public CppEntity, public CppForwardDeclarable {
    public:
        /// \returns An iteratable object iterating over the [cppast::CppFunctionParameter]()
        /// entities.
        detail::iteratable_intrusive_list<CppFunctionParameter> parameters() const noexcept {
            return type_safe::ref(parameters_);
        }

        /// \returns The [cppast::CppFunctionBodyKind]().
        /// \notes This matches the [cppast::CppForwardDeclarable]() queries.
        CppFunctionBodyKind body_kind() const noexcept {
            return body_;
        }

        /// \returns A [ts::optional_ref]() to the [cppast::CppExpression]() that is the given
        /// `noexcept` condition. \notes If this returns `nullptr`, the function has the implicit
        /// noexcept value (i.e. none, unless it is a destructor). \notes There is no way to distinguish
        /// between `noexcept` and `noexcept(true)`.
        type_safe::optional_ref<const CppExpression> noexcept_condition() const noexcept {
            return type_safe::opt_cref(noexcept_expr_.get());
        }

        /// \returns Whether the function has an ellipsis.
        bool is_variadic() const noexcept {
            return variadic_;
        }

        /// \returns The signature of the function,
        /// i.e. parameters and cv/ref-qualifiers if a member function.
        /// It has the form `(int,char,...) const`.
        std::string signature() const {
            return do_get_signature();
        }

    protected:
        /// Builder class for functions.
        ///
        /// Inherit from it to provide additional setter.
        template<typename T>
        class basic_builder {
        public:
            /// \effects Sets the name.
            basic_builder(std::string name) : function(new T(name)) {
            }

            /// \effects Adds a parameter.
            void add_parameter(std::unique_ptr<CppFunctionParameter> parameter) {
                static_cast<CppFunctionBase &>(*function).parameters_.push_back(*function,
                    std::move(parameter));
            }

            /// \effects Marks the function as variadic.
            void is_variadic() {
                static_cast<CppFunctionBase &>(*function).variadic_ = true;
            }

            /// \effects Sets the noexcept condition expression.
            void noexcept_condition(std::unique_ptr<CppExpression> cond) {
                static_cast<CppFunctionBase &>(*function).noexcept_expr_ = std::move(cond);
            }

            /// \returns The not yet finished function.
            T &get() noexcept {
                return *function;
            }

            /// \effects If the body is a definition, registers it.
            /// Else marks it as a declaration.
            /// \returns The finished function.
            std::unique_ptr<T> finish(const CppEntityIndex &idx, CppEntityId id,
                                      CppFunctionBodyKind body_kind,
                                      type_safe::optional<CppEntityRef> semantic_parent) {
                function->body_ = body_kind;
                function->set_semantic_parent(std::move(semantic_parent));
                if (cppast::is_definition(body_kind))
                    idx.register_definition(std::move(id), type_safe::ref(*function));
                else {
                    function->mark_declaration(id);
                    idx.register_forward_declaration(std::move(id), type_safe::ref(*function));
                }
                return std::move(function);
            }

            /// \returns The finished function without registering it.
            /// \notes This is intended for templated functions only.
            std::unique_ptr<T> finish(CppEntityId id, CppFunctionBodyKind body_kind,
                                      type_safe::optional<CppEntityRef> semantic_parent) {
                function->body_ = body_kind;
                function->set_semantic_parent(std::move(semantic_parent));
                if (!cppast::is_definition(body_kind))
                    function->mark_declaration(id);
                return std::move(function);
            }

        protected:
            basic_builder() = default;

            ~basic_builder() noexcept = default;

            std::unique_ptr<T> function;
        };

        CppFunctionBase(std::string name)
            : CppEntity(std::move(name)), body_(cpp_function_declaration), variadic_(false) {
        }

    protected:
        /// \returns The signature, it is called by [*signature()]().
        virtual std::string do_get_signature() const;

    private:
        detail::intrusive_list<CppFunctionParameter> parameters_;
        std::unique_ptr<CppExpression> noexcept_expr_;
        CppFunctionBodyKind body_;
        bool variadic_;
    };

    /// A [cppast::CppEntity]() modelling a C++ function.
    /// \notes This is not a member function,
    /// use [cppast::CppMemberFunction]() for that.
    /// It can be a `static` function of a class, however.
    class CppFunction final : public CppFunctionBase {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppFunction]().
        class builder : public CppFunctionBase::basic_builder<CppFunction> {
        public:
            /// \effects Sets the name and return type.
            builder(std::string name, std::unique_ptr<CppType> return_type) {
                function = std::unique_ptr<CppFunction>(
                    new CppFunction(std::move(name), std::move(return_type)));
            }

            /// \effects Sets the storage class.
            void storage_class(CppStorageClassSpecifiers storage) {
                function->storage_ = storage;
            }

            /// \effects Marks the function as `constexpr`.
            void is_constexpr() {
                function->constexpr_ = true;
            }

            /// \effects Marks the function as `consteval`.
            void is_consteval() {
                function->consteval_ = true;
            }
        };

        /// \returns A reference to the return [cppast::CppType]().
        const CppType &return_type() const noexcept {
            return *return_type_;
        }

        /// \returns The [cppast::cpp_storage_specifiers]() of the function.
        /// \notes If it is `cpp_storage_class_static` and inside a [cppast::CppClass](),
        /// it is a `static` class function.
        CppStorageClassSpecifiers storage_class() const noexcept {
            return storage_;
        }

        /// \returns Whether the function is marked `constexpr`.
        bool is_constexpr() const noexcept {
            return constexpr_;
        }

        /// \returns Whether the function is marked `consteval`.
        bool is_consteval() const noexcept {
            return consteval_;
        }

    private:
        CppEntityKind do_get_entity_kind() const noexcept override;

        CppFunction(std::string name, std::unique_ptr<CppType> ret)
            : CppFunctionBase(std::move(name)), return_type_(std::move(ret)),
              storage_(cpp_storage_class_auto), constexpr_(false), consteval_(false) {
        }

        std::unique_ptr<CppType> return_type_;
        CppStorageClassSpecifiers storage_;
        bool constexpr_;
        bool consteval_;
    };
} // namespace cppast
