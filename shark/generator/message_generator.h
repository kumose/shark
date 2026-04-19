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
#include <shark/ast/cpp_class.h>
#include <shark/generator/type_mapper.h>

namespace shark::generator {

    /// Generates a C++ class for a protobuf message.
    class MessageGenerator {
    public:
        MessageGenerator(const google::protobuf::Descriptor* msg,
                         cppast::CppEntityIndex& index);

        /// Generates the CppClass node.
        std::unique_ptr<cppast::CppEntity> generate();

    private:
        void add_member_variables();
        void add_getters();
        void add_setters();
        void add_from_proto_declaration();
        void add_to_proto_declaration();
        void add_nested_types();

        const google::protobuf::Descriptor* msg_;
        cppast::CppEntityIndex& index_;
        TypeMapper type_mapper_;
        cppast::CppClass::builder class_builder_;  // Will be initialized in generate()
    };

}  // namespace shark::generator