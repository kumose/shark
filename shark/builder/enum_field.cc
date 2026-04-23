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

#include <shark/builder/enum_field.h>
#include <shark/utility/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    // ===================================================================

    EnumFieldSkbGenerator::
    EnumFieldSkbGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
       variables_["type"] = descriptor->enum_type()->name();
    }

    EnumFieldSkbGenerator::~EnumFieldSkbGenerator() {
    }

    void EnumFieldSkbGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "$type$ _$name$$default_init$;\n");
                break;
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "std::vector<$type$> _$name$;\n");
                break;
        }
    }

    void EnumFieldSkbGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "_$name$ = rhs._$name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
                break;
        }
    }
    void EnumFieldSkbGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }


    void EnumFieldSkbGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "inline $type$ $domain_skb$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline void $domain_skb$::set_$name$($type$ value) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$ = value;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "inline const std::vector<$type$>& $domain_skb$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline std::vector<$type$> $domain_skb$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
        }
    }

    void EnumFieldSkbGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
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

    void EnumFieldSkbGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
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

    void EnumFieldSkbGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "$deprecated$inline $type$ $name$() const;\n");
                printer->Print(variables_, "inline void set_$name$($type$ value);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "$deprecated$inline const std::vector<$type$>& $name$() const;\n");
                printer->Print(variables_, "$deprecated$inline std::vector<$type$> mutable_$name$();\n");
                break;
        }
    }



    std::string EnumFieldSkbGenerator::do_get_default_value(void) const {
        const google::protobuf::EnumValueDescriptor* d = descriptor_->default_value_enum();
        if (!d) {
            return "";
        }
        std::string enum_type_name = descriptor_->enum_type()->name();
       return enum_type_name + "::" + d->name();
    }
    std::string EnumFieldSkbGenerator::get_default_value(void) const {
        return do_get_default_value();
    }

    void EnumFieldSkbGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {
    }

    void EnumFieldSkbGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {

    }
} // namespace shark
