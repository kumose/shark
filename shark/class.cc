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


#include <shark/class.h>
#include <shark/default_generator.h>

namespace cppast {
    ClassBuilder::ClassBuilder(std::string name, CppClassKind kind, bool is_final)
        : _id(get_seq_entity_id()), _builder(std::move(name), kind, is_final) {
    }

    ClassBuilder &ClassBuilder::add_child(std::unique_ptr<CppEntity> child) noexcept {
        _builder.add_child(std::move(child));
        return *this;
    }

    ClassBuilder &ClassBuilder::begin_public() noexcept {
        _builder.access_specifier(CppAccessSpecifierKind::cpp_public);
        return *this;
    }

    ClassBuilder &ClassBuilder::begin_protected() noexcept {
        _builder.access_specifier(CppAccessSpecifierKind::cpp_protected);
        return *this;

    }

    ClassBuilder &ClassBuilder::begin_private() noexcept {
        _builder.access_specifier(CppAccessSpecifierKind::cpp_private);
        return *this;
    }

    ClassBuilder &ClassBuilder::member_variable(std::string name,std::unique_ptr<CppType> type,
                                                        std::unique_ptr<CppExpression> def,
                                                        bool is_mutable) noexcept {
        CppEntityIndex idx;
        auto vid = get_seq_entity_id();
        return add_child(CppMemberVariable::build(idx, vid, std::move(name), std::move(type), std::move(def), is_mutable));
    }

    std::unique_ptr<CppClass> ClassBuilder::finish(type_safe::optional<CppEntityRef> semantic_parent) noexcept {
        CppEntityIndex idx;
        return _builder.finish(idx, _id, std::move(semantic_parent));
    }

    std::unique_ptr<CppClass> ClassBuilder::finish_declaration() noexcept {
        CppEntityIndex idx;
        return _builder.finish_declaration(idx, _id);
    }
} // namespace cppast
