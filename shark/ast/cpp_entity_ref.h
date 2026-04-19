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

#include <shark/ast/cpp_entity_index.h>
#include <shark/detail/assert.h>

namespace cppast {
    /// A basic reference to some kind of [cppast::CppEntity]().
    ///
    /// It can either refer to a single [cppast::CppEntity]()
    /// or multiple.
    /// In the later case it is *overloaded*.
    template<typename T, typename Predicate>
    class BasicCppEntityRef {
    public:
        /// \effects Creates it giving it the target id and name.
        BasicCppEntityRef(CppEntityId target_id, std::string target_name)
            : target_(std::move(target_id)), name_(std::move(target_name)) {
        }

        /// \effects Creates it giving it multiple target ids and name.
        /// \notes This is to refer to an overloaded function.
        BasicCppEntityRef(std::vector<CppEntityId> target_ids, std::string target_name)
            : target_(std::move(target_ids)), name_(std::move(target_name)) {
        }

        /// \returns The name of the reference, as spelled in the source code.
        const std::string &name() const noexcept {
            return name_;
        }

        /// \returns Whether or not it refers to multiple entities.
        bool is_overloaded() const noexcept {
            return target_.has_value(type_safe::variant_type<std::vector<CppEntityId> >{});
        }

        /// \returns The number of entities it refers to.
        type_safe::size_t no_overloaded() const noexcept {
            return id().size();
        }

        /// \returns An array reference to the id or ids it refers to.
        type_safe::array_ref<const CppEntityId> id() const noexcept {
            if (is_overloaded()) {
                auto &vec = target_.value(type_safe::variant_type<std::vector<CppEntityId> >{});
                return type_safe::ref(vec.data(), vec.size());
            } else {
                auto &id = target_.value(type_safe::variant_type<CppEntityId>{});
                return type_safe::ref(&id, 1u);
            }
        }

        /// \returns An array reference to the entities it refers to.
        /// The return type provides `operator[]` + `size()`,
        /// as well as `begin()` and `end()` returning forward iterators.
        /// \exclude return
        std::vector<type_safe::object_ref<const T> > get(const CppEntityIndex &idx) const {
            std::vector<type_safe::object_ref<const T> > result;
            get_impl(std::is_convertible<CppNamespace &, T &>{}, result, idx);
            return result;
        }

    private:
        void get_impl(std::true_type, std::vector<type_safe::object_ref<const T> > &result,
                      const CppEntityIndex &idx) const {
            for (auto &cur: id())
                for (auto &ns: idx.lookup_namespace(cur))
                    result.push_back(ns);
            if (!std::is_same<T, CppNamespace>::value)
                get_impl(std::false_type{}, result, idx);
        }

        void get_impl(std::false_type, std::vector<type_safe::object_ref<const T> > &result,
                      const CppEntityIndex &idx) const {
            for (auto &cur: id()) {
                auto entity = idx.lookup(cur).map([](const CppEntity &e) {
                    DEBUG_ASSERT(Predicate{}(e), detail::precondition_error_handler{},
                                 "invalid entity type");
                    return type_safe::ref(static_cast<const T &>(e));
                });
                if (entity)
                    result.push_back(type_safe::ref(entity.value()));
            }
        }

        type_safe::variant<CppEntityId, std::vector<CppEntityId> > target_;
        std::string name_;
    };

    /// \exclude
    namespace detail {
        struct cpp_entity_ref_predicate {
            bool operator()(const CppEntity &) {
                return true;
            }
        };
    } // namespace detail

    /// A [cppast::BasicCppEntityRef]() to any [cppast::CppEntity]().
    using CppEntityRef = BasicCppEntityRef<CppEntity, detail::cpp_entity_ref_predicate>;
} // namespace cppast
