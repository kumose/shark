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
#include <shark/generator/field_map.h>
#include <shark/rt/oneof_field.h>
#include <shark/generator/message.h>

namespace shark {
    class MessageGenerator : public MessageGeneratorBase {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit MessageGenerator(const google::protobuf::Descriptor *descriptor,
                                  const std::string &dllexport_decl);

        ~MessageGenerator() override;

        // Header stuff.

        // Generate typedef.
        void generate_fwd_typedef(google::protobuf::io::Printer *printer) override;

        // Generate definitions for this class and all its nested types.
        void generate_definition(google::protobuf::io::Printer *printer) override;

        void generate_inline_implement(google::protobuf::io::Printer *printer) override;

        // Source file stuff.
        void generate_static_variable(google::protobuf::io::Printer *printer) override ;

        void generate_static_functions(google::protobuf::io::Printer *printer) override;

        void generate_implement(google::protobuf::io::Printer *printer) override;

    private:
        // Generate definitions of all nested enums (must come before class
        // definitions because those classes use the enums definitions).
        void generate_enum_definitions(google::protobuf::io::Printer *printer);

        void generate_ctor_implemention(google::protobuf::io::Printer *printer);

        void generate_cpy_ctor_implemention(google::protobuf::io::Printer *printer);

        void generate_move_ctor_implement(google::protobuf::io::Printer *printer);

        void generate_trans_implement(google::protobuf::io::Printer *printer);

        void generate_serialize_implement(google::protobuf::io::Printer *printer);


        FieldGeneratorMap field_generators_;
        std::vector<std::unique_ptr<MessageGeneratorBase>> nested_generators_;
        std::vector<std::unique_ptr<EnumGenerator>> enum_generators_;
        std::vector<std::unique_ptr<ExtensionGenerator>> extension_generators_;
        std::map<std::string, std::string> _vars;
        std::vector<std::unique_ptr<OneofFieldGeneratorBase>> _oneof_generator;
    };
} // namespace shark
