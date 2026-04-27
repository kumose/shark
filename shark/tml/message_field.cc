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
#include <shark/tml/message_field.h>

namespace shark {
    MessageFieldGenerator::
    MessageFieldGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldNoMetaGenerator(descriptor) {
        _variables["type"] = descriptor_->message_type()->name();
        _variables["domain_type"] = message_type(descriptor->message_type());
    }

    MessageFieldGenerator::~MessageFieldGenerator() {
    }

    void MessageFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "$type$ $name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<$type$> $name$;\n");
                break;
        }
    }

    void MessageFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$name$ = std::move(rhs.$name$);\n");
    }
    void MessageFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$name$ = rhs.$name$;\n");
    }

    void MessageFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {

    }

    void MessageFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {

    }

    void MessageFieldGenerator::generate_trans_parse_toml_implementations(google::protobuf::io::Printer *printer) const {

        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(_variables, "uri.push_back(\"$name$\");\n");
                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "if (val->is_table()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$.parse_toml(*val, uri);\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is not table type\");\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is require as table type, but not exists\");\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();

                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK($name$.parse_toml(*val, uri));\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is require as table type, but not exists\");\n");
                printer->Outdent();
                printer->Print("}\n");
                break;

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "uri.push_back(\"$name$\");\n");
                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK($name$.parse_toml(*val, uri));\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$.clear();\n");
                printer->Print(_variables, "if (val->is_array()) {\n");
                printer->Indent();
                printer->Print(_variables, "int i = 0;\n");
                printer->Print(_variables, "for (auto& elem : val->as_array()) {\n");
                printer->Indent();
                printer->Print(_variables, "uri.push_back(turbo::str_format(\"$name$[%d]\", i++));\n");
                printer->Print(_variables, "$type$ tmp;\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(tmp.parse_toml(elem, uri));\n");
                printer->Print(_variables, "$name$.push_back(tmp);\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                break;
        }
    }


    void MessageFieldGenerator::generate_trans_toml_implementations(google::protobuf::io::Printer *printer) const {
        google::protobuf::SourceLocation fieldSourceLoc;
        descriptor_->GetSourceLocation(&fieldSourceLoc);

        auto n = fieldSourceLoc.leading_comments.size();
        n += fieldSourceLoc.trailing_comments.size();
        for (auto &it : fieldSourceLoc.leading_detached_comments) {
            n += it.size();
        }
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "auto var = $name$.serialize_toml();\n");
                if (n > 0) {
                    printer->Print("val.comments().push_back(\"#############################################\\n\"\n");
                    for (auto &it : fieldSourceLoc.leading_detached_comments) {
                        print_toml_comment(printer, it);
                    }
                    print_toml_comment(printer, fieldSourceLoc.leading_comments);
                    print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                    printer->Print("\"### end\\n\");\n");
                }
                printer->Print(_variables, "result[\"$name$\"] = var;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "shark::Value arr = shark::Array{};\n");
                if (n > 0) {
                    printer->Print("arr.comments().push_back(\"#############################################\\n\"\n");
                    for (auto &it : fieldSourceLoc.leading_detached_comments) {
                        print_toml_comment(printer, it);
                    }
                    print_toml_comment(printer, fieldSourceLoc.leading_comments);
                    print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                    printer->Print("\"### end\\n\");\n");
                }
                printer->Print(_variables, "for(size_t i = 0; i < $name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "arr.push_back($name$[i].serialize_toml());\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print(_variables, "result[\"$name$\"] = arr;\n");
                break;
        }
    }


    std::string MessageFieldGenerator::get_default_value(void) const {
        /* XXX: update when protobuf gets support
         *   for default-values of message fields.
         */
        return "";
    }



} // namespace shark
