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
#include <string>

#include <google/protobuf/descriptor.h>

#include <shark/ast/cpp_entity_index.h>
#include <shark/ast/cpp_file.h>

namespace shark::generator {

    /// Top-level generator for a .proto file.
    class FileGenerator {
    public:
        explicit FileGenerator(const google::protobuf::FileDescriptor* file);

        /// Generates the header file content.
        std::string generate_header();

        /// Generates the source file content.
        std::string generate_source();

    private:
        void add_includes(cppast::CppFile::builder& builder, bool is_header);
        void add_namespace(cppast::CppFile::builder& builder);
        void add_enum(const google::protobuf::EnumDescriptor* enum_desc);
        void add_message(const google::protobuf::Descriptor* msg);
        void build_ast();

        const google::protobuf::FileDescriptor* file_;
        cppast::CppEntityIndex index_;
        std::unique_ptr<cppast::CppFile> header_ast_;
        std::unique_ptr<cppast::CppFile> source_ast_;
    };

}  // namespace shark::generator