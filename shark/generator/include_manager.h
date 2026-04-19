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

#pragma once

#include <set>
#include <string>

#include <shark/ast/cpp_file.h>
#include <shark/ast/cpp_entity_index.h>

namespace shark::generator {

    class Type;

    /// Manages #include directives needed for generated code.
    class IncludeManager {
    public:
        /// Adds a specific header (e.g., "<string>", "<vector>").
        void add_include(const std::string& header);

        /// Adds includes required by a given Type.
        void add_type(const Type& type);

        /// Emits all collected includes to the given file builder.
        void emit_includes(cppast::CppFile::builder& builder) const;

    private:
        std::set<std::string> includes_;
    };

}  // namespace shark::generator
