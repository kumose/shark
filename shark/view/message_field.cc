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
#include <shark/view/message_field.h>

namespace shark {
    MessageFieldViewGenerator::
    MessageFieldViewGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
        _variables["type"] = relative_message_type(descriptor_->message_type(), descriptor_->containing_type(),"View") ;
        _variables["domain_type"] = message_type(descriptor->message_type(), "View");
    }

    MessageFieldViewGenerator::~MessageFieldViewGenerator() {
    }

    void MessageFieldViewGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "shark::MessageView<$type$> _$name$;\n");
    }

    void MessageFieldViewGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = std::move(rhs._$name$);\n");
    }

    void MessageFieldViewGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = rhs._$name$;\n");
    }

    void MessageFieldViewGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$deprecated$inline shark::MessageView<$type$> $name$() const;\n");
    }

    void MessageFieldViewGenerator::generate_members_inline_implementations(
        google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "inline shark::MessageView<$domain_type$> $domain_view$::$name$() const {\n");
        printer->Indent();
        printer->Print(_variables, "return _$name$;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
    }

    void MessageFieldViewGenerator::generate_trans_parse_pb_implementations(
        google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "_$name$.parse_from_proto(pb.$name$());\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$.resize(pb.$name$_size());");
                printer->Print(_variables, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$[i].parse_from_proto(pb.$name$(i));\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }


    void MessageFieldViewGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "_$name$.serialize_to_proto(*pb.mutable_$name$());\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(_variables, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$[i].serialize_to_proto(*pb.mutable_$name$()->Add());\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }


    std::string MessageFieldViewGenerator::get_default_value(void) const {
        /* XXX: update when protobuf gets support
         *   for default-values of message fields.
         */
        return "";
    }


} // namespace shark
