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

#include <typesafe/optional.hpp>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_file.h>

namespace cppast {

    class CppComment final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly built macro parameter.
        /// \notes It is not meant to be registered in the [cppast::CppEntityIndex]() as no other
        /// [cppast::CppEntity]() can refer to it.
        static std::unique_ptr<CppComment> build(std::string name) {
            return std::unique_ptr<CppComment>(new CppComment(std::move(name)));
        }

    private:
        CppComment(std::string name) : CppEntity(std::move(name)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;
    };

    /// A [cppast::CppEntity]() modelling a macro parameter.
    class CppMacroParameter final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly built macro parameter.
        /// \notes It is not meant to be registered in the [cppast::CppEntityIndex]() as no other
        /// [cppast::CppEntity]() can refer to it.
        static std::unique_ptr<CppMacroParameter> build(std::string name) {
            return std::unique_ptr<CppMacroParameter>(new CppMacroParameter(std::move(name)));
        }

    private:
        CppMacroParameter(std::string name) : CppEntity(std::move(name)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;
    };

    /// A [cppast::CppEntity]() modelling a macro definition.
    class CppMacroDefinition final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly built object like macro.
        /// \notes It is not meant to be registered in the [cppast::CppEntityIndex](),
        /// as no other [cppast::CppEntity]() can refer to it.
        static std::unique_ptr<CppMacroDefinition> build_object_like(std::string name,
                                                                     std::string replacement) {
            std::unique_ptr<CppMacroDefinition> result{new CppMacroDefinition(std::move(name))};
            result->replacement_ = std::move(replacement);
            return result;
        }

        /// Builds a function like macro.
        class FunctionLikeBuilder {
        public:
            /// \effects Sets the name of the function like macro.
            FunctionLikeBuilder(std::string name) : result_(new CppMacroDefinition(std::move(name))) {
                result_->kind_ = function_like;
            }

            /// \effects Sets the replacement text.
            void replacement(std::string replacement) {
                result_->replacement_ = std::move(replacement);
            }

            /// \effects Marks the macro as variadic.
            void is_variadic() {
                result_->kind_ = variadic_function;
            }

            /// \effects Adds a parameter.
            /// \group param
            void parameter(std::unique_ptr<CppMacroParameter> param) {
                result_->parameters_.push_back(*result_, std::move(param));
            }

            /// \group param
            void parameter(std::string name) {
                parameter(CppMacroParameter::build(std::move(name)));
            }

            /// \returns The finished macro.
            /// \notes It is not meant to be registered in the [cppast::CppEntityIndex](),
            /// as no other [cppast::CppEntity]() can refer to it.
            std::unique_ptr<CppMacroDefinition> finish() {
                return std::move(result_);
            }

        private:
            std::unique_ptr<CppMacroDefinition> result_;
        };

        /// \returns The replacement text of the macro.
        const std::string &replacement() const noexcept {
            return replacement_;
        }

        /// \returns Whether or not it is an object like macro.
        bool is_object_like() const noexcept {
            return kind_ == object_like;
        }

        /// \returns Whether or not it is a function like macro.
        bool is_function_like() const noexcept {
            return kind_ != object_like;
        }

        /// \returns Whether or not it is a variadic macro.
        bool is_variadic() const noexcept {
            return kind_ == variadic_function;
        }

        /// \returns The parameters of the macro.
        /// \notes It has none if it is not a function like macro.
        detail::iteratable_intrusive_list<CppMacroParameter> parameters() const noexcept {
            return type_safe::ref(parameters_);
        }

    private:
        CppEntityKind do_get_entity_kind() const noexcept override;

        CppMacroDefinition(std::string name) : CppEntity(std::move(name)), kind_(object_like) {
        }

        detail::intrusive_list<CppMacroParameter> parameters_;
        std::string replacement_;

        enum : char {
            object_like,
            function_like,
            variadic_function,
        } kind_;

        friend FunctionLikeBuilder;
    };

    /// The kind of [cppast::CppIncludeDirective]().
    enum class CppIncludeKind {
        system, //< An `#include <...>`.
        local, //< An `#include "..."`.
    };

    /// A [cppast::CppEntity]() modelling an `#include`.
    class CppIncludeDirective final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly built include directive.
        /// \notes It is not meant to be registered in the [cppast::CppEntityIndex](),
        /// as no other [cppast::CppEntity]() can refer to it.
        static std::unique_ptr<CppIncludeDirective> build(const CppFileRef &target,
                                                          CppIncludeKind kind,
                                                          std::string full_path) {
            return std::unique_ptr<CppIncludeDirective>(
                new CppIncludeDirective(target, kind, std::move(full_path)));
        }

        /// \returns A reference to the [cppast::CppFile]() it includes.
        CppFileRef target() const noexcept {
            return CppFileRef(target_, name());
        }

        /// \returns The kind of include it is.
        CppIncludeKind include_kind() const noexcept {
            return kind_;
        }

        /// \returns The full path of the included file.
        const std::string &full_path() const noexcept {
            return full_path_;
        }

    private:
        CppEntityKind do_get_entity_kind() const noexcept override;

        CppIncludeDirective(const CppFileRef &target, CppIncludeKind kind, std::string full_path)
            : CppEntity(target.name()), target_(target.id()[0u]), kind_(kind),
              full_path_(std::move(full_path)) {
            DEBUG_ASSERT(!target.is_overloaded(), detail::precondition_error_handler{});
        }

        CppEntityId target_;
        CppIncludeKind kind_;
        std::string full_path_;
    };
} // namespace cppast
