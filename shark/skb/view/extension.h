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

#include <string>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>

namespace shark {
    // Generates code for an extension, which may be within the scope of some
    // message or may be at file scope.  This is much simpler than FieldGenerator
    // since extensions are just simple identifiers with interesting types.
    class ExtensionViewGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit ExtensionViewGenerator(const google::protobuf::FieldDescriptor *descriptor,
                                    const std::string &dllexport_decl);

        ~ExtensionViewGenerator();

        // Header stuff.
        void GenerateDeclaration(google::protobuf::io::Printer *printer);

        // Source file stuff.
        void GenerateDefinition(google::protobuf::io::Printer *printer);

    private:
        const google::protobuf::FieldDescriptor *descriptor_;
        std::string type_traits_;
        std::string dllexport_decl_;
    };
} // namespace shark
