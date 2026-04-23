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

#include <shark/builder/bytes_field.h>
#include <shark/utility/helpers.h>

namespace shark {


    // ===================================================================

    BytesFieldSkbGenerator::
    BytesFieldSkbGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
    }

    BytesFieldSkbGenerator::~BytesFieldSkbGenerator() {
    }

    void BytesFieldSkbGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "std::vector<uint8> _$name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "std::vector<std::vector<uint8>> _$name$;\n");
                break;
        }
    }


    void BytesFieldSkbGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
    }
    void BytesFieldSkbGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }


    void BytesFieldSkbGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "$deprecated$inline const std::vector<uint8>& $name$() const;\n");
                printer->Print(variables_, "$deprecated$std::vector<uint8>& mutable_$name$();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "$deprecated$inline const std::vector<std::vector<uint8>> $name$() const;\n");
                printer->Print(variables_, "$deprecated$std::vector<std::vector<uint8>> mutable_$name$();\n");
                break;
        }
    }

    void BytesFieldSkbGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "inline const std::vector<uint8>& $domain_skb$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline std::vector<uint8>& $domain_skb$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$();\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "inline const std::vector<std::vector<uint8>> $domain_skb$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");

                printer->Print(variables_, "inline std::vector<std::vector<uint8>> $domain_skb$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
        }
    }

    void BytesFieldSkbGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "_$name$ = pb.$name$();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "_$name$.reserve(pb.$name$_size());");
                printer->Print(variables_, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$.push_back(pb.$name$(i));\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    void BytesFieldSkbGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "pb.set_$name$(_$name$);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(variables_, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(variables_, "*pb.mutable_$name$()->Add() = _$name$[i];\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }



    void BytesFieldSkbGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {

    }

    std::string BytesFieldSkbGenerator::get_default_value(void) const{
       return  "\"" + CEscape(descriptor_->default_value_string()) + "\"";
    }

    void BytesFieldSkbGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {

    }

    void BytesFieldSkbGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
