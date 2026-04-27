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
#include <shark/tml/string_field.h>

namespace shark {

    // ===================================================================

    StringFieldGenerator::
    StringFieldGenerator(const google::protobuf::FieldDescriptor *descriptor )
        : FieldNoMetaGenerator(descriptor) {
    }

    StringFieldGenerator::~StringFieldGenerator() {
    }

    void StringFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "std::string $name$$default_init$;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<std::string> $name$;\n");
                break;
        }
    }

    void StringFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
    }

    void StringFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {

    }

    void StringFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$name$ = std::move(rhs.$name$);\n");
    }
    void StringFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$name$ = rhs.$name$;\n");
    }

    void StringFieldGenerator::generate_trans_parse_toml_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(_variables, "uri.push_back(\"$name$\");\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "if (val->is_string()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$ = val->as_string();\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is not string type\");\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is require as string type, but not exists\");\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                break;

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "uri.push_back(\"$name$\");\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "std::optional<shark::Value> val = shark::find<shark::Value>(config, \"$name$\");\n");
                printer->Print(_variables, "if (val) {\n");
                printer->Indent();
                printer->Print(_variables, "if (val->is_string()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$ = val->as_string();\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is not string type\");\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print("}\n");
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
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "if (elem.is_string()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$.push_back(elem.as_string());\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"element of field $name$ is not string type\");\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "uri.push_back(\"$name$[0]\");\n");
                printer->Print(_variables, "uri.pop_back();\n");
                printer->Print(_variables, "if (val->is_string()) {\n");
                printer->Indent();
                printer->Print(_variables, "$name$.clear();\n");
                printer->Print(_variables, "$name$.push_back(val->as_string());\n");
                printer->Outdent();
                printer->Print(_variables, "} else {\n");
                printer->Indent();
                printer->Print(_variables, "return turbo::invalid_argument_error(\"field $name$ is not string or array of string\");\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    void StringFieldGenerator::generate_trans_toml_implementations(google::protobuf::io::Printer *printer) const {
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
                printer->Print(_variables, "shark::Value val = $name$;\n");
                if (n > 0) {
                    printer->Print("val.comments().push_back(\"#############################################\\n\"\n");
                    for (auto &it : fieldSourceLoc.leading_detached_comments) {
                        print_toml_comment(printer, it);
                    }
                    print_toml_comment(printer, fieldSourceLoc.leading_comments);
                    print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                    printer->Print("\"### end\\n\");\n");
                }
                printer->Print(_variables, "result[\"$name$\"] = val;\n");
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
                printer->Print(_variables, "arr.push_back($name$[i]);\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print(_variables, "result[\"$name$\"] = arr;\n");
                break;
        }
    }


    void StringFieldGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> vars;
        vars["default"] = _variables.find("default")->second;
        vars["escaped"] = CEscape(descriptor_->default_value_string());
        printer->Print(vars, "char $default$[] = \"$escaped$\";\n");
    }

    std::string StringFieldGenerator::do_get_default_value(void) const {
        std::string d;
        if (!turbo::c_decode(std::string_view(descriptor_->default_value_string().data(), descriptor_->default_value_string().size()), &d)) {
            KLOG(FATAL) << "Can't get default value for $default$ by:" << descriptor_->default_value_string();
        }
        return d;
    }
    std::string StringFieldGenerator::get_default_value(void) const {
        return turbo::str_format("{\"%s\"}", do_get_default_value());
    }

} // namespace shark
