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


#include <shark/namespace.h>
#include <shark/ast/cpp_preprocessor.h>
#include <shark/default_generator.h>

namespace cppast {

    NamespaceBuilder::NamespaceBuilder(std::string name, bool is_inline, bool is_nested)
        :_id(get_seq_entity_id()),_builder(name, is_inline, is_nested) {

    }

    NamespaceBuilder &NamespaceBuilder::add_child(std::unique_ptr<CppEntity> child) noexcept {
        _builder.add_child(std::move(child));
        return *this;
    }

    NamespaceBuilder &NamespaceBuilder::comment(std::string_view comment) noexcept {
        return add_child(CppComment::build(std::string(comment)));
    }

    std::unique_ptr<CppNamespace> NamespaceBuilder::finish() {
        cppast::CppEntityIndex idx;
        return _builder.finish(idx, _id);
    }
}  // namespace cppast
