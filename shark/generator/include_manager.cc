// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
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

#include <shark/generator/include_manager.h>
#include <shark/generator/type.h>
#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_preprocessor.h>
#include <shark/ast/cpp_attribute.h>
#include <shark/ast/cpp_type.h>

namespace shark::generator {

    void IncludeManager::add_include(const std::string& header) {
        includes_.insert(header);
    }

    void IncludeManager::add_type(const Type& type) {
        switch (type.kind()) {
            case Type::Kind::k_string:
                add_include("<string>");
                break;
            case Type::Kind::k_vector:
                add_include("<vector>");
                // Also add includes for the element type
                for (const auto& param : type.parameters()) {
                    add_type(param);
                }
                break;
            case Type::Kind::k_unordered_map:
                add_include("<unordered_map>");
                for (const auto& param : type.parameters()) {
                    add_type(param);
                }
                break;
            default:
                // Other types (built-in, user-defined, enum) don't require extra includes
                break;
        }
    }

    void IncludeManager::emit_includes(cppast::CppFile::builder& builder) const {
        for (const auto& header : includes_) {
            // Create a CppIncludeDirective node and add to the file builder.
            // The target is a dummy CppFileRef (not needed for actual generation, just the name).
            auto include = cppast::CppIncludeDirective::build(
                cppast::CppFileRef(cppast::CppEntityId(header), header),  // target (not used)
                cppast::CppIncludeKind::system,
                header);
            builder.add_child(std::move(include));
        }
    }

}  // namespace shark::generator