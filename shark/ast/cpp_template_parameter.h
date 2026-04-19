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
#include <typesafe/variant.hpp>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_variable_base.h>
#include <shark/detail/intrusive_list.hpp>

namespace cppast {
    /// Base class for all entities modelling a template parameter of some kind.
    class CppTemplateParameter : public CppEntity {
    public:
        /// \returns Whether or not the parameter is variadic.
        bool is_variadic() const noexcept {
            return variadic_;
        }

    protected:
        CppTemplateParameter(std::string name, bool variadic)
            : CppEntity(std::move(name)), variadic_(variadic) {
        }

    private:
        bool variadic_;
    };

    /// The kind of keyword used in a template parameter.
    enum class cpp_template_keyword {
        keyword_class,
        keyword_typename,
        concept_contraint
    };

    /// \returns The string associated of the keyword.
    const char *to_string(cpp_template_keyword kw) noexcept;

    /// A [cppast::CppEntity]() modelling a C++ template type parameter.
    class CppTemplateTypeParameter final : public CppTemplateParameter {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered template type parameter.
        /// \notes The `default_type` may be `nullptr` in which case the parameter has no default.
        static std::unique_ptr<CppTemplateTypeParameter> build(
            const CppEntityIndex &idx, CppEntityId id, std::string name, cpp_template_keyword kw,
            bool variadic, std::unique_ptr<CppType> default_type = nullptr,
            type_safe::optional<CppTokenString> concept_constraint = type_safe::nullopt);

        /// \returns A [ts::optional_ref]() to the default type.
        type_safe::optional_ref<const CppType> default_type() const noexcept {
            return type_safe::opt_cref(default_type_.get());
        }

        /// \returns The keyword used in the template parameter.
        cpp_template_keyword keyword() const noexcept {
            return keyword_;
        }

        const type_safe::optional<CppTokenString> &concept_constraint() const noexcept {
            return concept_constraint_;
        }

    private:
        CppTemplateTypeParameter(std::string name, cpp_template_keyword kw, bool variadic,
                                 std::unique_ptr<CppType> default_type,
                                 type_safe::optional<CppTokenString> concept_constraint)
            : CppTemplateParameter(std::move(name), variadic), default_type_(std::move(default_type)),
              keyword_(kw), concept_constraint_(concept_constraint) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppType> default_type_;
        cpp_template_keyword keyword_;
        type_safe::optional<CppTokenString> concept_constraint_;
    };

    /// \exclude
    namespace detail {
        struct cpp_template_parameter_ref_predicate {
            bool operator()(const CppEntity &e);
        };
    } // namespace detail

    /// Reference to a [cppast::CppTemplateTypeParameter]().
    using CppTemplateTypeParameterRef
    = BasicCppEntityRef<CppTemplateTypeParameter,
        detail::cpp_template_parameter_ref_predicate>;

    /// A [cppast::CppType]() defined by a [cppast::CppTemplateTypeParameter]().
    class CppTemplateParameterType final : public CppType {
    public:
        /// \returns A newly created parameter type.
        static std::unique_ptr<CppTemplateParameterType> build(
            CppTemplateTypeParameterRef parameter) {
            return std::unique_ptr<CppTemplateParameterType>(
                new CppTemplateParameterType(std::move(parameter)));
        }

        /// \returns A reference to the [cppast::CppTemplateTypeParameter]() this type refers to.
        const CppTemplateTypeParameterRef &entity() const noexcept {
            return parameter_;
        }

    private:
        CppTemplateParameterType(CppTemplateTypeParameterRef parameter)
            : parameter_(std::move(parameter)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::template_parameter_t;
        }

        CppTemplateTypeParameterRef parameter_;
    };

    /// A [cppast::CppEntity]() modelling a C++ non-type template parameter.
    class CppNonTypeTemplateParameter final : public CppTemplateParameter,
                                              public CppVariableBase {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created and registered non type template parameter.
        /// \notes The `default_value` may be `nullptr` in which case the parameter has no default.
        static std::unique_ptr<CppNonTypeTemplateParameter> build(
            const CppEntityIndex &idx, CppEntityId id, std::string name,
            std::unique_ptr<CppType> type, bool is_variadic,
            std::unique_ptr<CppExpression> default_value = nullptr);

    private:
        CppNonTypeTemplateParameter(std::string name, std::unique_ptr<CppType> type, bool variadic,
                                    std::unique_ptr<CppExpression> def)
            : CppTemplateParameter(std::move(name), variadic),
              CppVariableBase(std::move(type), std::move(def)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;
    };

    /// \exclude
    namespace detail {
        struct CppTemplateRefPredicate {
            bool operator()(const CppEntity &e);
        };
    } // namespace detail

    /// A reference to a [cppast::CppTemplate]() or a [cppast::CppTemplateTemplateParameter]().
    using CppTemplateRef = BasicCppEntityRef<CppEntity, detail::CppTemplateRefPredicate>;

    /// A [cppast::CppEntity]() modelling a C++ template template parameter.
    class CppTemplateTemplateParameter final : public CppTemplateParameter {
    public:
        static CppEntityKind kind() noexcept;

        /// Builds a [cppast::CppTemplateTemplateParameter]().
        class builder {
        public:
            /// \effects Sets the name and whether it is variadic.
            builder(std::string name, bool variadic)
                : parameter_(new CppTemplateTemplateParameter(std::move(name), variadic)) {
            }

            /// \effects Sets the keyword,
            /// default is [cpp_template_keyword::keyword_class]().
            void keyword(cpp_template_keyword kw) {
                parameter_->keyword_ = kw;
            }

            /// \effects Adds a parameter to the template.
            void add_parameter(std::unique_ptr<CppTemplateParameter> param) {
                parameter_->parameters_.push_back(*parameter_, std::move(param));
            }

            /// \effects Sets the default template.
            void default_template(CppTemplateRef templ) {
                parameter_->default_ = std::move(templ);
            }

            /// \effects Registers the parameter in the [cppast::CppEntityIndex](),
            /// using the given [cppast::CppEntityId]().
            /// \returns The finished parameter.
            std::unique_ptr<CppTemplateTemplateParameter> finish(const CppEntityIndex &idx,
                                                                 CppEntityId id) {
                idx.register_definition(std::move(id), type_safe::ref(*parameter_));
                return std::move(parameter_);
            }

        private:
            std::unique_ptr<CppTemplateTemplateParameter> parameter_;
        };

        /// \returns An iteratable object containing the template parameters of the template template
        /// parameter.
        detail::iteratable_intrusive_list<CppTemplateParameter> parameters() const noexcept {
            return type_safe::ref(parameters_);
        }

        /// \returns The keyword used in the template parameter.
        cpp_template_keyword keyword() const noexcept {
            return keyword_;
        }

        /// \returns A [ts::optional]() that is the default template.
        type_safe::optional<CppTemplateRef> default_template() const noexcept {
            return default_;
        }

    private:
        CppTemplateTemplateParameter(std::string name, bool variadic)
            : CppTemplateParameter(std::move(name), variadic),
              keyword_(cpp_template_keyword::keyword_class) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        detail::intrusive_list<CppTemplateParameter> parameters_;
        type_safe::optional<CppTemplateRef> default_;
        cpp_template_keyword keyword_;
    };

    /// An argument for a [cppast::CppTemplateParameter]().
    ///
    /// It is based on a [ts::variant]() of [cppast::CppType]() (for
    /// [cppast::CppTemplateTypeParameter]()), [cppast::CppExpression]() (for
    /// [cppast::CppNonTypeTemplateParameter]()) and [cppast::CppTemplateRef]() (for
    /// [cppast::CppTemplateTemplateParameter]().
    class CppTemplateArgument {
    public:
        /// \effects Initializes it passing a type as argument.
        /// This corresponds to a [cppast::CppTemplateTypeParameter]().
        /// \notes This constructor only participates in overload resolution if `T` is dervied from
        /// [cppast::CppType](). \param 1 \exclude
        template<typename T,
            typename std::enable_if<std::is_base_of<CppType, T>::value, int>::type = 0>
        CppTemplateArgument(std::unique_ptr<T> type)
            : arg_(std::unique_ptr<CppType>(std::move(type))) {
        }

        /// \effects Initializes it passing an expression as argument.
        /// This corresponds to a [cppast::CppNonTypeTemplateParameter]().
        /// \notes This constructor only participates in overload resolution if `T` is dervied from
        /// [cppast::CppExpression](). \param 1 \exclude
        template<typename T,
            typename = typename std::enable_if<std::is_base_of<CppExpression, T>::value>::type>
        CppTemplateArgument(std::unique_ptr<T> expr)
            : arg_(std::unique_ptr<CppExpression>(std::move(expr))) {
        }

        /// \effects Initializes it passing a template as argument.
        /// This corresponds to a [cppast::CppTemplateTemplateParameter]().
        CppTemplateArgument(CppTemplateRef templ) : arg_(std::move(templ)) {
        }

        type_safe::optional_ref<const CppType> type() const noexcept {
            return arg_.optional_value(type_safe::variant_type<std::unique_ptr<CppType> >{})
                    .map([](const std::unique_ptr<CppType> &type) { return type_safe::ref(*type); });
        }

        type_safe::optional_ref<const CppExpression> expression() const noexcept {
            return arg_.optional_value(type_safe::variant_type<std::unique_ptr<CppExpression> >{})
                    .map([](const std::unique_ptr<CppExpression> &expr) { return type_safe::ref(*expr); });
        }

        type_safe::optional_ref<const CppTemplateRef> template_ref() const noexcept {
            return arg_.optional_value(type_safe::variant_type<CppTemplateRef>{});
        }

    private:
        type_safe::variant<std::unique_ptr<CppType>, std::unique_ptr<CppExpression>, CppTemplateRef>
        arg_;
    };
} // namespace cppast
