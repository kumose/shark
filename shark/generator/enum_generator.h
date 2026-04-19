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

#pragma once

#include <memory>

#include <google/protobuf/descriptor.h>

#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_entity.h>

namespace shark::generator {

    /// Generates a C++ enum class from a protobuf EnumDescriptor.
    class EnumGenerator {
    public:
        explicit EnumGenerator(const google::protobuf::EnumDescriptor* enum_desc);

        /// Generates the CppEnum node.
        /// The caller is responsible for adding the returned node to the appropriate parent
        /// (e.g., namespace or class).
        std::unique_ptr<cppast::CppEntity> generate(cppast::CppEntityIndex& index);

    private:
        const google::protobuf::EnumDescriptor* enum_desc_;
    };

}  // namespace shark::generator
