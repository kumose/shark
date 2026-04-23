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

namespace shark {
    class EnumViewGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit EnumViewGenerator(const google::protobuf::EnumDescriptor *descriptor,
                               const std::string &dllexport_decl);

        ~EnumViewGenerator();

        // Header stuff.

        // Generate header code defining the enum.  This code should be placed
        // within the enum's package namespace, but NOT within any class, even for
        // nested enums.
        void generate_definition(google::protobuf::io::Printer *printer, const google::protobuf::Descriptor* parent);

        void generate_inline_declarations(google::protobuf::io::Printer *printer, const google::protobuf::Descriptor *descriptor);

        void generate_inline_definition(google::protobuf::io::Printer *printer, const google::protobuf::Descriptor *descriptor);

        // void GenerateDescriptorDeclarations(google::protobuf::io::Printer *printer);


        // Source file stuff.

    private:
        const google::protobuf::EnumDescriptor *descriptor_;
        std::string _dllexport_decl;
    };
} // namespace shark
