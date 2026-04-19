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
#include <shark/ast/cpp_entity_container.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// A [cppast::CppEntity]() representing a friend declaration.
    ///
    /// It can either declare or define a `friend` function (template), declare a `friend` class,
    /// or refer to an existing type.
    class CppFriend : public CppEntity, private CppEntityContainer<CppFriend, CppEntity> {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created friend declaring the given entity as `friend`.
        /// \notes The friend declaration itself will not be registered,
        /// but the referring entity is.
        static std::unique_ptr<CppFriend> build(std::unique_ptr<CppEntity> e) {
            return std::unique_ptr<CppFriend>(new CppFriend(std::move(e)));
        }

        /// \returns A newly created friend declaring the given type as `friend`.
        /// \notes It will not be registered.
        static std::unique_ptr<CppFriend> build(std::unique_ptr<CppType> type) {
            return std::unique_ptr<CppFriend>(new CppFriend(std::move(type)));
        }

        /// \returns An optional reference to the entity it declares as friend, or `nullptr`.
        type_safe::optional_ref<const CppEntity> entity() const noexcept {
            if (begin() == end())
                return nullptr;
            return type_safe::ref(*begin());
        }

        /// \returns An optional reference to the type it declares as friend, or `nullptr`.
        type_safe::optional_ref<const CppType> type() const noexcept {
            return type_safe::opt_ref(type_.get());
        }

    private:
        CppFriend(std::unique_ptr<CppEntity> e) : CppEntity("") {
            add_child(std::move(e));
        }

        CppFriend(std::unique_ptr<CppType> type) : CppEntity(""), type_(std::move(type)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppType> type_;

        friend CppEntityContainer<CppFriend, CppEntity>;
    };
} // namespace cppast
