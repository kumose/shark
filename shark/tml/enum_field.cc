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

#include <shark/tml/enum_field.h>
#include <shark/utility/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    // TODO(kenton):  Factor out a "SetCommonFieldVariables()" to get rid of
    //   repeat code between this and the other field types.
    void SetEnumVariables(const google::protobuf::FieldDescriptor *descriptor,
                          std::map<std::string, std::string> *variables) {
        (*variables)["type"] = descriptor->enum_type()->name();
        std::string full = descriptor->enum_type()->full_name();
        auto n = descriptor->enum_type()->file()->package();
        auto pre = turbo::strip_prefix(full, n + ".");
        pre = turbo::strip_suffix(full, descriptor->enum_type()->full_name());

        std::string prefix;
        if (pre.empty()) {
            prefix = GlobalState::instance().cnamespace + "::";
        } else {
            prefix = turbo::str_replace_all(pre, {{".", "::"}}) + "::";
        }
        (*variables)["PREFIX"] = prefix;
    }

    // ===================================================================

    EnumFieldGenerator::
    EnumFieldGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldNoMetaGenerator(descriptor) {
        SetEnumVariables(descriptor, &_variables);
    }

    EnumFieldGenerator::~EnumFieldGenerator() {
    }

    void EnumFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "$type$ $name$$default_init$;\n");
                break;
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<$type$> $name$;\n");
                break;
        }
    }

    void EnumFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "$name$ = rhs.$name$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "$name$ = std::move(rhs.$name$);\n");
                break;
        }
    }

    void EnumFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$name$ = rhs.$name$;\n");
    }


    void EnumFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
    }

    void EnumFieldGenerator::generate_trans_parse_toml_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "TURBO_MOVE_OR_RAISE(std::string str, xtoml::find_key<std::string>(config, \"$name$\"));\n");
                printer->Print(_variables, "if (!str.empty()) {\n");
                printer->Indent();
                printer->Print(_variables, "auto tmp = $PREIX$parse_$type$(str);\n");
                printer->Print(_variables, "if (tmp) {\n");
                printer->Indent();
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&*tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = *tmp;\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(
                    _variables,
                    "return turbo::invalid_argument_error(\"field $name$ is not enum type, got\", val->as_string());\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print("}\n");
                break;

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "std::string str = xtoml::find_key(config, \"$name$\", \"\");\n");
                printer->Print(_variables, "if (!str.empty()) {\n");
                printer->Indent();
                printer->Print(_variables, "auto tmp = $PREIX$parse_$type$(str);\n");
                printer->Print(_variables, "if (tmp) {\n");
                printer->Indent();
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&*tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = *tmp;\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(
                    _variables,
                    "return turbo::invalid_argument_error(\"field $name$ is not enum type, got\", str);\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED: {
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(
                    _variables, "std::vector<std::string> arrs;\n");
                if (!_required) {
                    printer->Print(_variables, "TURBO_RETURN_NOT_OK(xtoml::find_key_array(config, \"$name$\", arrs).ignore_not_found_error());\n");
                } else {
                    printer->Print(_variables, "TURBO_RETURN_NOT_OK(xtoml::find_key_array(config, \"$name$\", arrs));\n");
                }
                printer->Print(_variables, "if (!arrs.empty()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$.clear();\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "for (size_t i =0; i < arrs.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "xtoml::TomlUriGuard lg(uri, {xtoml::FieldKey(i)});\n");
                printer->Print(_variables, "auto tmp = $PREIX$parse_$type$(str);\n");
                printer->Print(_variables, "if (tmp) {\n");
                printer->Indent();
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check_element(i, &*tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");

                printer->Print(_variables, "$name$ = *tmp;\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(
                    _variables,
                    "return turbo::invalid_argument_error(\"field $name$ is not enum type, got\", val->as_string());\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check($name$));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
            }
        }
    }

    void EnumFieldGenerator::generate_trans_toml_implementations(google::protobuf::io::Printer *printer, bool required) const {
        google::protobuf::SourceLocation fieldSourceLoc;
        descriptor_->GetSourceLocation(&fieldSourceLoc);

        auto n = fieldSourceLoc.leading_comments.size();
        n += fieldSourceLoc.trailing_comments.size();
        for (auto &it: fieldSourceLoc.leading_detached_comments) {
            n += it.size();
        }
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(_variables, "xtoml::Value var = $PREFIX$to_string($name$);\n");
                if (n > 0) {
                    printer->Print("var.comments().push_back(\"#############################################\\n\"\n");
                    for (auto &it: fieldSourceLoc.leading_detached_comments) {
                        print_toml_comment(printer, it);
                    }
                    print_toml_comment(printer, fieldSourceLoc.leading_comments);
                    print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                    printer->Print("\"### end\\n\");\n");
                }
                printer->Print(_variables, "result[\"$name$\"] = var;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (!required) {
                    printer->Print(_variables, "xtoml::Value var = $PREFIX$to_string($name$);\n");
                    if (n > 0) {
                        printer->Print("var.comments().push_back(\"#############################################\\n\"\n");
                        for (auto &it: fieldSourceLoc.leading_detached_comments) {
                            print_toml_comment(printer, it);
                        }
                        print_toml_comment(printer, fieldSourceLoc.leading_comments);
                        print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                        printer->Print("\"### end\\n\");\n");
                    }
                    printer->Print(_variables, "result[\"$name$\"] = var;\n");
                }
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                if (!required || _required) {
                    printer->Print(_variables, "xtoml::Value arr = xtoml::Array{};\n");
                    if (n > 0) {
                        printer->Print("arr.comments().push_back(\"#############################################\\n\"\n");
                        for (auto &it: fieldSourceLoc.leading_detached_comments) {
                            print_toml_comment(printer, it);
                        }
                        print_toml_comment(printer, fieldSourceLoc.leading_comments);
                        print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                        printer->Print("\"### end\\n\");\n");
                    }
                    printer->Print(_variables, "for(size_t i = 0; i < $name$.size(); ++i) {\n");
                    printer->Indent();
                    printer->Print(_variables, "arr.push_back($PREFIX$to_string($name$[i]));\n");
                    printer->Outdent();
                    printer->Print("}\n");
                    printer->Print(_variables, "result[\"$name$\"] = arr;\n");
                }
                break;
        }
    }

    void EnumFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
    }


    std::string EnumFieldGenerator::do_get_default_value(void) const {
        const google::protobuf::EnumValueDescriptor *d = descriptor_->default_value_enum();
        if (!d) {
            return "";
        }
        std::string enum_type_name = descriptor_->enum_type()->name();
        return turbo::str_format("{%s::%s}", enum_type_name, d->name());
    }

    std::string EnumFieldGenerator::get_default_value(void) const {
        return do_get_default_value();
    }
} // namespace shark
