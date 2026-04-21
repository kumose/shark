// SPDX-License-Identifier: BSD-3-Clause
// Based on Google Protobuf (https://github.com/protocolbuffers/protobuf) and protobuf-c
// (https://github.com/protobuf-c/protobuf-c)
// Copyright 2008 Google Inc., 2008-2025 protobuf-c authors. Modifications for C++ generation.
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
#include <turbo/strings/str_cat.h>
#include <shark/utility/uri.h>
#include <shark/rt/message_field.h>

namespace shark {
    MessageFieldGenerator::
    MessageFieldGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
        variables_["type"] = message_domain_without_namespace(descriptor_);
    }

    MessageFieldGenerator::~MessageFieldGenerator() {
    }

    void MessageFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "$type$ _$name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "std::vector<$type$> _$name$;\n");
                break;
        }
    }

    void MessageFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
    }
    void MessageFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }

    void MessageFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "$deprecated$ inline const $type$& $name$() const;\n");
                printer->Print(variables_, "$deprecated$ inline void set_$name$(const $type$& val);\n");
                printer->Print(variables_, "$deprecated$ inline void set_$name$($type$&& val);\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "$deprecated$ inline const std::vector<$type$>& $name$() const;\n");
                printer->Print(variables_, "$deprecated$ std::vector<$type$>& mutable_$name$();\n");
                break;
        }
    }

    void MessageFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "inline const $type$& $domain$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline void $domain$::set_$name$(const $type$& val) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$ = val;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline void $domain$::set_$name$($type$&& val) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$ = std::move(val);\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(
                    variables_, "inline const std::vector<$type$>& $domain$::$name$() const {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                printer->Print(variables_, "inline std::vector<$type$>& $domain$::mutable_$name$() {\n");
                printer->Indent();
                printer->Print(variables_, "return _$name$;\n");
                printer->Outdent();
                printer->Print(variables_, "}\n");
                break;
        }
    }

    void MessageFieldGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "_$name$.parse_from_proto(pb.$name$());\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "_$name$.resize(pb.$name$_size());");
                printer->Print(variables_, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$[i].parse_from_proto(pb.$name$(i));\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }


    void MessageFieldGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(variables_, "_$name$.serialize_to_proto(*pb.mutable_$name$());\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables_, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(variables_, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(variables_, "_$name$[i].serialize_to_proto(*pb.mutable_$name$()->Add());\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }


    std::string MessageFieldGenerator::get_default_value(void) const {
        /* XXX: update when protobuf gets support
         *   for default-values of message fields.
         */
        return "";
    }

    void MessageFieldGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {

    }

    void MessageFieldGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {

    }
} // namespace shark
