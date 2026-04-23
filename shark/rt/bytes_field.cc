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



#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>

#include <shark/rt/bytes_field.h>
#include <shark/utility/helpers.h>

namespace shark {


    // ===================================================================

    BytesFieldGenerator::
    BytesFieldGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
    }

    BytesFieldGenerator::~BytesFieldGenerator() {
    }

    void BytesFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "std::vector<uint8> _$name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<std::vector<uint8>> _$name$;\n");
                break;
        }
    }


    void BytesFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = std::move(rhs._$name$);\n");
    }
    void BytesFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = rhs._$name$;\n");
    }


    void BytesFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "$deprecated$inline const std::vector<uint8>& $name$() const;\n");
                printer->Print(_variables, "$deprecated$std::vector<uint8>& mutable_$name$();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "$deprecated$inline const std::vector<std::vector<uint8>> $name$() const;\n");
                printer->Print(_variables, "$deprecated$std::vector<std::vector<uint8>> mutable_$name$();\n");
                break;
        }
    }

    void BytesFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "inline const std::vector<uint8>& $domain$::$name$() const {\n");
                printer->Indent();
                printer->Print(_variables, "return _$name$;\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "inline std::vector<uint8>& $domain$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(_variables, "return _$name$();\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "inline const std::vector<std::vector<uint8>> $domain$::$name$() const {\n");
                printer->Indent();
                printer->Print(_variables, "return _$name$;\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");

                printer->Print(_variables, "inline std::vector<std::vector<uint8>> $domain$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(_variables, "return _$name$;\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                break;
        }
    }

    void BytesFieldGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "_$name$ = pb.$name$();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$.reserve(pb.$name$_size());");
                printer->Print(_variables, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$.push_back(pb.$name$(i));\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    void BytesFieldGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "pb.set_$name$(_$name$);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(_variables, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "*pb.mutable_$name$()->Add() = _$name$[i];\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }



    void BytesFieldGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {

    }

    std::string BytesFieldGenerator::get_default_value(void) const{
       return  "\"" + CEscape(descriptor_->default_value_string()) + "\"";
    }

    void BytesFieldGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {

    }

    void BytesFieldGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
