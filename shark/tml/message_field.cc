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
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "TURBO_MOVE_OR_RAISE(auto const *val, try_find_key(config, \"$name$\", val));\n");
                printer->Print(_variables, "$type$ tmp;\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(tmp.parse_toml(*val, uri, map));\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = std::move(tmp);\n");
                break;

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");

                printer->Print(_variables, "auto rs = xtoml::find_key_table(config, \"$name$\");\n");
                printer->Print(_variables, "if (rs.ok()) {\n");
                printer->Indent();
                printer->Print(_variables, "$type$ tmp;\n");
                printer->Print(_variables, "auto val = std::move(rs).value_or_die();\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(tmp.parse_toml(*val, uri, map));\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = std::move(tmp);\n");
                printer->Outdent();
                printer->Print(_variables, "} else if (!turbo::is_not_found(rs.status())) {\n");
                printer->Indent();
                printer->Print(_variables, "return rs.status();\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "auto rs = xtoml::find_key_array(config, \"$name$\");\n");

                printer->Print(_variables, "if (rs.ok()) {\n");
                printer->Indent();
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "std::vector<$type$> tarr;\n");
                printer->Print(_variables, "xtoml::Array arrs = rs.value_or_die()->as_array();\n");
                printer->Print(_variables, "for (size_t i = 0; i < arrs.size(); i++) {\n");
                printer->Indent();
                printer->Print(_variables, "xtoml::TomlUriGuard lg(uri, {xtoml::FieldKey(i)});\n");
                printer->Print(_variables, "$type$ tmp;\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(tmp.parse_toml(arrs[i], uri, map));\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check_element(i, &tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "tarr.push_back(std::move(tmp));\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tarr));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = std::move(tarr);\n");
                printer->Outdent();

                printer->Print(_variables, "} else if (!turbo::is_not_found(rs.status())) {\n");
                printer->Indent();
                printer->Print(_variables, "return rs.status();\n");
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
                printer->Print(_variables, "xtoml::Value arr = xtoml::Array{};\n");
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
