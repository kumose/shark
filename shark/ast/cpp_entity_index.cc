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




#include <shark/ast/cpp_entity_index.h>

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_entity_kind.h>
#include <shark/ast/cpp_file.h>
#include <shark/detail/assert.h>

using namespace cppast;

CppEntityIndex::DuplicateDefinitionError::DuplicateDefinitionError()
    : std::logic_error("duplicate registration of entity definition") {
}

void CppEntityIndex::register_definition(CppEntityId id,
                                         type_safe::object_ref<const CppEntity> entity) const {
    DEBUG_ASSERT(entity->kind() != CppEntityKind::namespace_t,
                 detail::precondition_error_handler{}, "must not be a namespace");
    std::lock_guard<std::mutex> lock(mutex_);
    auto result = map_.emplace(std::move(id), value(entity, true));
    if (!result.second) {
        // already in map, override declaration
        auto &value = result.first->second;
        if (value.is_definition && !is_template(value.entity->kind()) && entity->parent()
            && !is_template(entity->parent().value().kind()))
            // allow duplicate definition of templates
            // this handles things such as SFINAE
            throw DuplicateDefinitionError();
        value.is_definition = true;
        value.entity = entity;
    }
}

bool CppEntityIndex::register_file(CppEntityId id,
                                   type_safe::object_ref<const CppFile> file) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return map_.emplace(std::move(id), value(file, true)).second;
}

void CppEntityIndex::register_forward_declaration(
    CppEntityId id, type_safe::object_ref<const CppEntity> entity) const {
    std::lock_guard<std::mutex> lock(mutex_);
    map_.emplace(std::move(id), value(entity, false));
}

void CppEntityIndex::register_namespace(CppEntityId id,
                                        type_safe::object_ref<const CppNamespace> ns) const {
    std::lock_guard<std::mutex> lock(mutex_);
    ns_[std::move(id)].push_back(ns);
}

type_safe::optional_ref<const CppEntity> CppEntityIndex::lookup(
    const CppEntityId &id) const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = map_.find(id);
    if (iter == map_.end())
        return {};
    return type_safe::ref(iter->second.entity.get());
}

type_safe::optional_ref<const CppEntity> CppEntityIndex::lookup_definition(
    const CppEntityId &id) const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = map_.find(id);
    if (iter == map_.end() || !iter->second.is_definition)
        return {};
    return type_safe::ref(iter->second.entity.get());
}

auto CppEntityIndex::lookup_namespace(const CppEntityId &id) const noexcept
    -> type_safe::array_ref<type_safe::object_ref<const CppNamespace> > {
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = ns_.find(id);
    if (iter == ns_.end())
        return nullptr;
    auto &vec = iter->second;
    return type_safe::ref(vec.data(), vec.size());
}
