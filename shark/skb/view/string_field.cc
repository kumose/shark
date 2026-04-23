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
#include <shark/skb/view/string_field.h>

namespace shark {
    // ===================================================================

    StringFieldViewGenerator::
    StringFieldViewGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
    }

    StringFieldViewGenerator::~StringFieldViewGenerator() {
    }

    void StringFieldViewGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "shark::StringView _$name$;\n");
    }

    void StringFieldViewGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "$deprecated$inline shark::StringView $name$() const;\n");
    }

    void StringFieldViewGenerator::generate_members_inline_implementations(
        google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "/// getter\n");
        printer->Print(variables_, "inline shark::StringView $domain_view$::$name$() const {\n");
        printer->Indent();
        printer->Print(variables_, "return _$name$;\n");
        printer->Outdent();
        printer->Print(variables_, "}\n");
    }

    void StringFieldViewGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
    }

    void StringFieldViewGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }

    void StringFieldViewGenerator::generate_trans_parse_pb_implementations(
        google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "_$name$ = pb.$name$();\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "_$name$.reserve(pb.$name$_size());\n");
                printer->Print(variables_, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$.push_back(pb.$name$(i));\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    void StringFieldViewGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
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


    void StringFieldViewGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> vars;
        vars["default"] = variables_.find("default")->second;
        vars["escaped"] = CEscape(descriptor_->default_value_string());
        printer->Print(vars, "char $default$[] = \"$escaped$\";\n");
    }

    std::string StringFieldViewGenerator::do_get_default_value(void) const {
        std::string d;
        if (!turbo::c_decode(std::string_view(descriptor_->default_value_string().data(),
                                              descriptor_->default_value_string().size()), &d)) {
            KLOG(FATAL) << "Can't get default value for $default$ by:" << descriptor_->default_value_string();
        }
        return d;
    }

    std::string StringFieldViewGenerator::get_default_value(void) const {
        return do_get_default_value();
    }

    void StringFieldViewGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {
    }

    void StringFieldViewGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
