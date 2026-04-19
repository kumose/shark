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

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <typesafe/optional_ref.hpp>
#include <typesafe/reference.hpp>
#include <typesafe/strong_typedef.hpp>

#include <shark/ast/cppast_fwd.h>

namespace cppast {
    /// \exclude
    namespace detail {
        using hash_type = std::uint_least64_t;
        constexpr hash_type fnv_basis = 14695981039346656037ull;
        constexpr hash_type fnv_prime = 1099511628211ull;

        // FNV-1a 64 bit hash
        constexpr hash_type id_hash(const char *str, hash_type hash = fnv_basis) {
            return *str ? id_hash(str + 1, (hash ^ hash_type(*str)) * fnv_prime) : hash;
        }
    } // namespace detail

    /// A [ts::strong_typedef]() representing the unique id of a [cppast::CppEntity]().
    ///
    /// It is comparable for equality.
    struct CppEntityId : type_safe::strong_typedef<CppEntityId, detail::hash_type>,
                         type_safe::strong_typedef_op::equality_comparison<CppEntityId> {
        explicit CppEntityId(const std::string &str) : CppEntityId(str.c_str()) {
        }

        explicit CppEntityId(const char *str) : strong_typedef(detail::id_hash(str)) {
        }
    };

    inline namespace literals {
        /// \returns A new [cppast::CppEntityId]() created from the given string.
        inline CppEntityId operator"" _id(const char *str, std::size_t) {
            return CppEntityId(str);
        }
    } // namespace literals

    /// An index of all [cppast::CppEntity]() objects created.
    ///
    /// It maps [cppast::CppEntityId]() to references to the [cppast::CppEntity]() objects.
    class CppEntityIndex {
    public:
        /// Exception thrown on duplicate entity definition.
        class DuplicateDefinitionError : public std::logic_error {
        public:
            DuplicateDefinitionError();
        };

        /// \effects Registers a new [cppast::CppEntity]() which is a definition.
        /// It will override any previously registered declarations of the same entity.
        /// \throws duplicate_defintion_error if the entity has been registered as definition before.
        /// \requires The entity must live as long as the index lives,
        /// and it must not be a namespace.
        /// \notes This operation is thread safe.
        void register_definition(CppEntityId id,
                                 type_safe::object_ref<const CppEntity> entity) const;

        /// \effects Registers a new [cppast::CppFile]().
        /// \returns `true` if the file was not registered before.
        /// If it returns `false`, the file was registered before and nothing was changed.
        /// \requires The entity must live as long as the index lives.
        /// \notes This operation is thread safe.
        bool register_file(CppEntityId id, type_safe::object_ref<const CppFile> file) const;

        /// \effects Registers a new [cppast::CppEntity]() which is a declaration.
        /// Only the first declaration will be registered.
        /// \requires The entity must live as long as the index lives.
        /// \requires The entity must be forward declarable.
        /// \notes This operation is thread safe.
        void register_forward_declaration(CppEntityId id,
                                          type_safe::object_ref<const CppEntity> entity) const;

        /// \effects Registers a new [cppast::CppNamespace]().
        /// \notes The namespace object must live as long as the index lives.
        /// \notes This operation is thread safe.
        void register_namespace(CppEntityId id, type_safe::object_ref<const CppNamespace> ns) const;

        /// \returns A [ts::optional_ref]() corresponding to the entity(/ies) of the given
        /// [cppast::CppEntityId](). If no definition has been registered, it return the first
        /// declaration that was registered. If the id resolves to a namespaces, returns an empty
        /// optional. \notes This operation is thread safe.
        type_safe::optional_ref<const CppEntity> lookup(const CppEntityId &id) const noexcept;

        /// \returns A [ts::optional_ref]() corresponding to the entity of the given
        /// [cppast::CppEntityId](). If no definition has been registered, it returns an empty
        /// optional. \notes This operation is thread safe.
        type_safe::optional_ref<const CppEntity> lookup_definition(
            const CppEntityId &id) const noexcept;

        /// \returns A [ts::array_ref]() of references to all namespaces matching the given
        /// [cppast::CppEntityId](). If no namespace is found, it returns an empty array reference.
        /// \notes This operation is thread safe.
        auto lookup_namespace(const CppEntityId &id) const noexcept
            -> type_safe::array_ref<type_safe::object_ref<const CppNamespace> >;

    private:
        struct hash {
            std::size_t operator()(const CppEntityId &id) const noexcept {
                return std::size_t(static_cast<detail::hash_type>(id));
            }
        };

        struct value {
            type_safe::object_ref<const CppEntity> entity;
            bool is_definition;

            value(type_safe::object_ref<const CppEntity> e, bool def)
                : entity(std::move(e)), is_definition(def) {
            }
        };

        mutable std::mutex mutex_;
        mutable std::unordered_map<CppEntityId, value, hash> map_;
        mutable std::unordered_map<CppEntityId,
            std::vector<type_safe::object_ref<const CppNamespace> >, hash>
        ns_;
    };
} // namespace cppast
