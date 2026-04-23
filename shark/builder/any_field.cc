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



#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <shark/builder/any_field.h>

namespace shark {
    // ===================================================================

    AnyFieldSkbGenerator::
    AnyFieldSkbGenerator(const google::protobuf::FieldDescriptor *descriptor )
        : FieldMetaGenerator(descriptor) {
    }

    AnyFieldSkbGenerator::~AnyFieldSkbGenerator() {
    }

    void AnyFieldSkbGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "std::pair<std::string,std::string> _$name$$default_init$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<std::pair<std::string,std::string>> _$name$;\n");
                break;
        }
    }

    void AnyFieldSkbGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "$deprecated$inline void set_$name$(const std::pair<std::string,std::string> &v);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "$deprecated$inline std::vector<std::pair<std::string,std::string>>& mutable_$name$();\n");
                break;
        }
    }

    void AnyFieldSkbGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "/// setter\n");
                printer->Print(_variables, "inline void $domain_skb$::set_$name$(const std::pair<std::string,std::string> &v) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$ = v;\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "/// setter\n");
                printer->Print(_variables, "inline std::vector<std::pair<std::string,std::string>>& $domain_skb$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(_variables, "return _$name$;\n");
                printer->Outdent();
                printer->Print(_variables, "}\n\n");
                break;
        }
    }

    void AnyFieldSkbGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = std::move(rhs._$name$);\n");
    }
    void AnyFieldSkbGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = rhs._$name$;\n");
    }

    void AnyFieldSkbGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "_$name$.first = pb.$name$().type_url();\n");
                printer->Print(_variables, "_$name$.second = pb.$name$().value();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$.resize(pb.$name$_size());\n");
                printer->Print(_variables, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$[i].first = pb.$name$(i).type_url();\n");
                printer->Print(_variables, "_$name$[i].second = pb.$name$(i).value();\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    void AnyFieldSkbGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "pb.mutable_$name$()->set_type_url(_$name$.first);\n");
                printer->Print(_variables, "pb.mutable_$name$()->set_value(_$name$.second);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(_variables, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "auto ptr = pb.mutable_$name$()->Add();\n");
                printer->Print(_variables, "ptr->set_type_url(_$name$[i].first);\n");
                printer->Print(_variables, "ptr->set_value(_$name$[i].second);\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }


    void AnyFieldSkbGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> vars;
        vars["default"] = _variables.find("default")->second;
        vars["escaped"] = CEscape(descriptor_->default_value_string());
        printer->Print(vars, "char $default$[] = \"$escaped$\";\n");
    }


    std::string AnyFieldSkbGenerator::get_default_value(void) const {
        return "";
    }

    void AnyFieldSkbGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {

    }

    void AnyFieldSkbGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
