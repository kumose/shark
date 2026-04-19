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


#include <atomic>
#include <string>

#include <typesafe/optional_ref.hpp>

#include <shark/ast/cpp_attribute.h>
#include <shark/ast/cpp_token.h>
#include <shark/detail/intrusive_list.hpp>

namespace cppast {
    /// The name of a scope.
    ///
    /// It is a combination of a name and optional template parameters.
    class CppScopeName {
    public:
        /// \effects Creates a scope out of a given entity.
        CppScopeName(type_safe::object_ref<const CppEntity> entity);

        /// \returns The name of the scope.
        const std::string &name() const noexcept;

        /// \returns Whether or not the scope is templated.
        bool is_templated() const noexcept {
            return templ_.has_value();
        }

        /// \returns An iteratable object iterating over the [cppast::CppTemplateParameter]() entities
        /// of the scope. \requires The scope is templated.
        detail::iteratable_intrusive_list<CppTemplateParameter> template_parameters() const noexcept;

    private:
        type_safe::object_ref<const CppEntity> entity_;
        type_safe::optional_ref<const CppTemplate> templ_;
    };

    /// The base class for all entities in the C++ AST.
    class CppEntity : detail::intrusive_list_node<CppEntity> {
    public:
        CppEntity(const CppEntity &) = delete;

        CppEntity &operator=(const CppEntity &) = delete;

        virtual ~CppEntity() noexcept = default;

        /// \returns The kind of the entity.
        CppEntityKind kind() const noexcept {
            return do_get_entity_kind();
        }

        /// \returns The name of the entity.
        /// The name is the string associated with the entity's declaration.
        const std::string &name() const noexcept {
            return name_;
        }

        /// \returns The name of the new scope created by the entity,
        /// if there is any.
        type_safe::optional<CppScopeName> scope_name() const {
            return do_get_scope_name();
        }

        /// \returns A [ts::optional_ref]() to the parent entity in the AST.
        type_safe::optional_ref<const CppEntity> parent() const noexcept {
            return parent_;
        }

        /// \returns The documentation comment associated with that entity, if any.
        /// \notes A documentation comment can have three forms:
        ///
        /// * A C style doc comment. It is a C style comment starting with an additional `*`, i.e.
        /// `/**`. One space after the leading sequence will be skipped. It ends either with `*/` or
        /// `**/`. After a newline all whitespace is skipped, as well as an optional `*` followed by
        /// another optional space, as well as trailing whitespace on each line. I.e. `/** a\n      * b
        /// */` yields the text `a\nb`.
        /// * A C++ style doc comment. It is a C++ style comment starting with an additional `/` or '!`,
        /// i.e. `///` or `//!`.
        /// One space character after the leading sequence will be skipped,
        /// as well as any trailing whitespace.
        /// Two C++ style doc comments on two adjacent lines will be merged.
        /// * An end of line doc comment. It is a C++ style comment starting with an '<', i.e. `//<`.
        /// One space character after the leading sequence will be skipped,
        /// as well as any trailing whitespace.
        /// If the next line is a C++ style doc comment, it will be merged with that one.
        ///
        /// A documentation comment is associated with an entity,
        /// if for C and C++ style doc comments, the entity declaration begins
        /// on the line after the last line of the comment,
        /// and if for an end of line comment, the entity declaration ends
        /// on the same line as the end of line comment.
        ///
        /// This comment system is also used by [standardese](https://standardese.foonathan.net).
        type_safe::optional_ref<const std::string> comment() const noexcept {
            return comment_.empty() ? nullptr : type_safe::opt_ref(&comment_);
        }

        /// \effects Sets the associated comment.
        /// \requires The comment must not be empty, if there is one.
        void set_comment(type_safe::optional<std::string> comment) noexcept {
            comment_ = comment.value_or("");
        }

        /// \returns The list of attributes that are specified for that entity.
        const cpp_attribute_list &attributes() const noexcept {
            return attributes_;
        }

        /// \effects Adds an attribute for that entity.
        void add_attribute(CppAttribute attr) noexcept {
            attributes_.push_back(std::move(attr));
        }

        /// \effects Adds multiple arguments for that entity.
        void add_attribute(const cpp_attribute_list &list) noexcept {
            attributes_.insert(attributes_.end(), list.begin(), list.end());
        }

        /// \returns The specified user data.
        void *user_data() const noexcept {
            return user_data_.load();
        }

        /// \effects Sets some kind of user data.
        ///
        /// User data is just some kind of pointer, there are no requirements.
        /// The class will do no lifetime management.
        ///
        /// User data is useful if you need to store additional data for an entity without the need to
        /// maintain a registry.
        void set_user_data(void *data) const noexcept {
            user_data_ = data;
        }

        /// \effects Creates it giving it the the name.
        CppEntity(std::string name) : name_(std::move(name)), user_data_(nullptr) {
        }

    private:
        /// \returns The kind of the entity.
        virtual CppEntityKind do_get_entity_kind() const noexcept = 0;

        /// \returns The name of the new scope created by the entity, if any.
        /// By default, there is no scope created.
        virtual type_safe::optional<CppScopeName> do_get_scope_name() const {
            return type_safe::nullopt;
        }

        void on_insert(const CppEntity &parent) noexcept {
            parent_ = type_safe::ref(parent);
        }

        std::string name_;
        std::string comment_;
        cpp_attribute_list attributes_;
        type_safe::optional_ref<const CppEntity> parent_;
        mutable std::atomic<void *> user_data_;

        template<typename T>
        friend struct detail::intrusive_list_access;
        friend detail::intrusive_list_node<CppEntity>;
    };

    /// A [cppast::CppEntity]() that isn't exposed directly.
    ///
    /// The only information available is the raw source code.
    class CppUnexposedEntity final : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly built and registered unexposed entity.
        /// \notes It will be registered as a declaration.
        static std::unique_ptr<CppEntity> build(const CppEntityIndex &index, CppEntityId id,
                                                 std::string name, CppTokenString spelling);

        /// \returns A newly built unnamed unexposed entity.
        /// It will not be registered.
        static std::unique_ptr<CppEntity> build(CppTokenString spelling);

        /// \returns The spelling of that entity.
        const CppTokenString &spelling() const noexcept {
            return spelling_;
        }

    private:
        CppUnexposedEntity(std::string name, CppTokenString spelling)
            : CppEntity(std::move(name)), spelling_(std::move(spelling)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        CppTokenString spelling_;
    };

    /// \returns Whether or not the entity is templated.
    /// If this function returns `true` that means the entity is not the "real" entity,
    /// but contains just the information for the template which is the parent entity.
    /// \notes Do not use this entity other to read information from the template entity.
    bool is_templated(const CppEntity &e) noexcept;

    /// \returns Whether or not the given entity is "friended",
    /// that is, its declaration exists as part of a [cppast::CppFriend]() declaration.
    bool is_friended(const CppEntity &e) noexcept;
} // namespace cppast
