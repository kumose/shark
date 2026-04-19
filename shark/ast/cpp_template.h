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

#include <vector>

#include <typesafe/variant.hpp>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_template_parameter.h>
#include <shark/ast/cpp_token.h>

namespace cppast {
    /// Base class for all entities modelling a C++ template of some kind, aside from concepts
    ///
    /// It is a container of a single [cppast::CppEntity]() that is the entity being templated.
    class CppTemplate : public CppEntity, public CppEntityContainer<CppTemplate, CppEntity> {
    public:
        /// \returns An iteratable object iterating over the [cppast::CppTemplateParameter]()
        /// entities. \notes These may be empty for a full specialization.
        detail::iteratable_intrusive_list<CppTemplateParameter> parameters() const noexcept {
            return type_safe::ref(parameters_);
        }

    protected:
        /// Builder class for templates.
        ///
        /// Inherit from it to provide additional setter.
        template<class T, class EntityT>
        class basic_builder {
        public:
            /// \effects Sets the entity that is begin templated.
            basic_builder(std::unique_ptr<EntityT> templ) : template_entity(new T(std::move(templ))) {
            }

            basic_builder(basic_builder &&) = default;

            /// \effects Adds a parameter.
            void add_parameter(std::unique_ptr<CppTemplateParameter> parameter) {
                static_cast<CppTemplate &>(*template_entity)
                        .parameters_.push_back(*template_entity, std::move(parameter));
            }

            /// \returns The not yet finished template.
            T &get() const noexcept {
                return *template_entity;
            }

            /// \effects Registers the template.
            /// \returns The finished template.
            std::unique_ptr<T> finish(const CppEntityIndex &idx, CppEntityId id, bool is_definition) {
                if (is_definition)
                    idx.register_definition(std::move(id), type_safe::cref(*template_entity));
                else
                    idx.register_forward_declaration(std::move(id), type_safe::cref(*template_entity));
                return std::move(template_entity);
            }

        protected:
            basic_builder() = default;

            ~basic_builder() noexcept = default;

            std::unique_ptr<T> template_entity;
        };

        /// \effects Sets the entity to be templated.
        CppTemplate(std::unique_ptr<CppEntity> entity) : CppEntity(entity->name()) {
            add_child(std::move(entity));
        }

    private:
        type_safe::optional<cppast::CppScopeName> do_get_scope_name() const override {
            return begin()->scope_name()
                       ? type_safe::make_optional(cppast::CppScopeName(type_safe::ref(*this)))
                       : type_safe::nullopt;
        }

        detail::intrusive_list<CppTemplateParameter> parameters_;
    };

    /// A [cppast::CppType]() representing an instantiation of a [cppast::CppTemplate]().
    class CppTemplateInstantiationType final : public CppType {
    public:
        /// Builds a [cppast::cpp_template_instantiation]().
        class builder {
        public:
            /// \effects Sets the primary template being instantiated.
            builder(CppTemplateRef templ)
                : result_(new CppTemplateInstantiationType(std::move(templ))) {
            }

            /// \effects Adds the next argument.
            /// \requires No call to `add_unexposed_arguments()` has happened before.
            void add_argument(CppTemplateArgument arg) {
                result_->arguments_.value(type_safe::variant_type<std::vector<CppTemplateArgument> >{})
                        .push_back(std::move(arg));
            }

            /// \effects Adds unexposed arguments as string.
            void add_unexposed_arguments(std::string arg) {
                result_->arguments_ = std::move(arg);
            }

            /// \returns The finished instantiation.
            std::unique_ptr<CppTemplateInstantiationType> finish() {
                return std::move(result_);
            }

        private:
            std::unique_ptr<CppTemplateInstantiationType> result_;
        };

        /// \returns A reference to the template that is being instantiated.
        /// \notes It could also point to a specialization,
        /// this is just the *primary* template.
        const CppTemplateRef &primary_template() const noexcept {
            return templ_;
        }

        /// \returns Whether or not the arguments are exposed.
        bool arguments_exposed() const noexcept {
            return arguments_.has_value(type_safe::variant_type<std::vector<CppTemplateArgument> >{});
        }

        /// \returns An array ref to the [cppast::CppTemplateArgument](), if there are any.
        /// \requires The arguments are exposed, i.e. `arguments_exposed()` returns `true`.
        type_safe::optional<type_safe::array_ref<const CppTemplateArgument> > arguments()
        const noexcept {
            auto &vec = arguments_.value(type_safe::variant_type<std::vector<CppTemplateArgument> >{});
            if (vec.empty())
                return type_safe::nullopt;
            return type_safe::ref(vec.data(), vec.size());
        }

        /// \returns The unexposed arguments as string.
        /// \requires The arguments are not exposed, i.e. `arguments_exposed()` returns `false`.
        const std::string &unexposed_arguments() const noexcept {
            return arguments_.value(type_safe::variant_type<std::string>{});
        }

    private:
        CppTemplateInstantiationType(CppTemplateRef ref)
            : arguments_(type_safe::variant_type<std::vector<CppTemplateArgument> >{}),
              templ_(std::move(ref)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::template_instantiation_t;
        }

        type_safe::variant<std::vector<CppTemplateArgument>, std::string> arguments_;
        CppTemplateRef templ_;
    };

    /// Base class for all entities modelling a C++ template specialization.
    class CppTemplateSpecialization : public CppTemplate {
    public:
        /// \returns A reference to the template that is being specialized.
        CppTemplateRef primary_template() const noexcept {
            return CppTemplateRef(templ_, name());
        }

        /// \returns Whether or not the arguments are exposed.
        bool arguments_exposed() const noexcept {
            return arguments_.has_value(type_safe::variant_type<std::vector<CppTemplateArgument> >{});
        }

        /// \returns An iteratable object iterating over the [cppast::CppTemplateArgument]()s.
        /// \requires The arguments are exposed, i.e. `arguments_exposed()` returns `true`.
        /// \notes For function template specializations it can be empty,
        /// meaning that the arguments are not explictly given but deduced from the signature.
        type_safe::array_ref<const CppTemplateArgument> arguments() const noexcept {
            auto &vec = arguments_.value(type_safe::variant_type<std::vector<CppTemplateArgument> >{});
            return type_safe::ref(vec.data(), vec.size());
        }

        /// \returns The unexposed arguments as string.
        /// \requires The arguments are not exposed, i.e. `arguments_exposed()` returns `false`.
        /// \notes For function template specializations it can be empty,
        /// meaning that the arguments are not explictly given but deduced from the signature.
        const CppTokenString &unexposed_arguments() const noexcept {
            return arguments_.value(type_safe::variant_type<CppTokenString>{});
        }

        /// \returns Whether or not the specialization is a full specialization.
        bool is_full_specialization() const noexcept {
            // if no template parameters are given, it is a full specialization
            return parameters().empty();
        }

    protected:
        /// Builder class for specializations.
        ///
        /// Inherit from it to provide additional setter.
        template<class T, class EntityT>
        class specialization_builder : public basic_builder<T, EntityT> {
        public:
            /// \effects Sets the entity that is being templated and the primary template.
            specialization_builder(std::unique_ptr<EntityT> entity, const CppTemplateRef &templ) {
                this->template_entity = std::unique_ptr<T>(new T(std::move(entity), templ));
            }

            /// \effects Adds the next argument for the [cppast::CppTemplateParameter]() of the
            /// primary template. \requires No call to `add_unexposed_arguments()` has happened before.
            void add_argument(CppTemplateArgument arg) {
                auto &specialization
                        = static_cast<CppTemplateSpecialization &>(*this->template_entity);
                specialization.arguments_
                        .value(type_safe::variant_type<std::vector<CppTemplateArgument> >{})
                        .push_back(std::move(arg));
            }

            /// \effects Adds unexposed arguments as string.
            void add_unexposed_arguments(CppTokenString arg) {
                auto &specialization
                        = static_cast<CppTemplateSpecialization &>(*this->template_entity);
                specialization.arguments_ = std::move(arg);
            }

        protected:
            specialization_builder() = default;
        };

        /// \effects Sets the entity that is being templated and the primary template.
        CppTemplateSpecialization(std::unique_ptr<CppEntity> entity, const CppTemplateRef &templ)
            : CppTemplate(std::move(entity)),
              arguments_(type_safe::variant_type<std::vector<CppTemplateArgument> >{}),
              templ_(templ.id()[0u]) {
            DEBUG_ASSERT(!templ.is_overloaded()
                         && (templ.name().empty() || templ.name() == begin()->name()),
                         detail::precondition_error_handler{}, "invalid name of template ref");
        }

    private:
        type_safe::variant<std::vector<CppTemplateArgument>, CppTokenString> arguments_;
        CppEntityId templ_;
    };
} // namespace cppast
