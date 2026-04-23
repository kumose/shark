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


#include <memory>
#include <string>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/rt/oneof_field.h>
#include <shark/rt/enum.h>
#include <shark/rt/extension.h>
#include <shark/rt/field_map.h>

namespace shark {
    class MessageGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit MessageGenerator(const google::protobuf::Descriptor *descriptor,
                                  const std::string &dllexport_decl,const MessageGenerator *parent);

        ~MessageGenerator();

        // Header stuff.

        // Generate typedef.
        void generate_struct_typedef(google::protobuf::io::Printer *printer);

        void generate_struct_inl(google::protobuf::io::Printer *printer);

        // Generate definitions for this class and all its nested types.
        void generate_struct_definition(google::protobuf::io::Printer *printer);

        // Source file stuff.
        void generate_struct_transfer(google::protobuf::io::Printer *printer);
        // Generate code that initializes the global variable storing the message's
        // descriptor.
        void GenerateMessageDescriptor(google::protobuf::io::Printer *printer);

        bool have_oneof() const {
            return _have_one_of;
        }
    private:
        // Generate definitions of all nested enums (must come before class
        // definitions because those classes use the enums definitions).
        void generate_enum_definitions(google::protobuf::io::Printer *printer);

        const google::protobuf::Descriptor *descriptor_;
        std::string dllexport_decl_;
        FieldGeneratorMap field_generators_;
        std::vector<std::unique_ptr<MessageGenerator>> nested_generators_;
        std::vector<std::unique_ptr<EnumGenerator>> enum_generators_;
        std::vector<std::unique_ptr<ExtensionGenerator>> extension_generators_;
        bool _have_one_of;
        std::map<std::string, std::string> _vars;
        OneofFieldGenerator _oneof_generator;
    };
} // namespace shark
