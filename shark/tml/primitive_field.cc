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

#include <shark/utility/helpers.h>
#include <shark/tml/primitive_field.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>

namespace shark {
    static std::string toml_type(google::protobuf::FieldDescriptor::Type type) {
        switch (type) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
                return "integer";
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
                return "boolean";
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                return "floating";
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_STRING:
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                TURBO_UNREACHABLE();

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    PrimitiveFieldGenerator::
    PrimitiveFieldGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldNoMetaGenerator(descriptor) {
        std::string c_type = get_ctype(descriptor_, descriptor->containing_type());
        _variables["c_type"] = c_type;
        is_atomic = _ext_option.is_atomic();
        _variables["toml_type"] = toml_type(descriptor_->type());
    }

    PrimitiveFieldGenerator::~PrimitiveFieldGenerator() {
    }

    void PrimitiveFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "std::atomic<$c_type$> $name$$default_init$;\n");
                } else {
                    printer->Print(_variables, "$c_type$ $name$$default_init$;\n");
                }

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "std::vector<$c_type$> $name$;\n");
                break;
        }
    }

    void PrimitiveFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
    }

    void PrimitiveFieldGenerator::generate_members_inline_implementations(
        google::protobuf::io::Printer *printer) const {
    }


    void PrimitiveFieldGenerator::generate_trans_parse_toml_implementations(
        google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "TURBO_MOVE_OR_RAISE(auto tmp, xtoml::find_key(config, \"$name$\"));\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = tmp;\n");
                break;

            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "auto tmp = xtoml::find_key(config, \"$name$\", $name$);\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = tmp;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "xtoml::TomlUriGuard gard(uri, {xtoml::FieldKey(\"$name$\")});\n");
                printer->Print(_variables, "std::vector<$c_type$> tmp;\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(xtoml::find_key_array(config, \"$name$\", tmp).ignore_not_found_error());\n");
                printer->Print(_variables, "auto checker = xtoml::find_handler(map, uri);\n");
                printer->Print(_variables, "if(checker) {\n");
                printer->Indent();
                printer->Print(_variables, "for(size_t i = 0; i < tmp.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "xtoml::TomlUriGuard lg(uri, {xtoml::FieldKey(i)});\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check_element(i, &tmp[i]));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "TURBO_RETURN_NOT_OK(checker->check(&tmp));\n");
                printer->Outdent();
                printer->Print(_variables, "}\n");
                printer->Print(_variables, "$name$ = std::move(tmp);\n");
                break;
        }
    }

    void PrimitiveFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "$name$.store(rhs.$name$.load());\n");
                } else {
                    printer->Print(_variables, "$name$ = rhs.$name$;\n");
                }

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "$name$ = std::move(rhs.$name$);\n");
                break;
        }
    }

    void PrimitiveFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "$name$.store(rhs.$name$.load());\n");
                } else {
                    printer->Print(_variables, "$name$ = rhs.$name$;\n");
                }

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "$name$ = rhs.$name$;\n");
                break;
        }
    }

    void PrimitiveFieldGenerator::generate_trans_toml_implementations(google::protobuf::io::Printer *printer) const {
        google::protobuf::SourceLocation fieldSourceLoc;
        descriptor_->GetSourceLocation(&fieldSourceLoc);

        auto n = fieldSourceLoc.leading_comments.size();
        n += fieldSourceLoc.trailing_comments.size();
        for (auto &it: fieldSourceLoc.leading_detached_comments) {
            n += it.size();
        }

        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                printer->Print(_variables, "xtoml::Value val = $name$;\n");
                if (n > 0) {
                    printer->Print("val.comments().push_back(\"#############################################\\n\"\n");
                    for (auto &it: fieldSourceLoc.leading_detached_comments) {
                        print_toml_comment(printer, it);
                    }
                    print_toml_comment(printer, fieldSourceLoc.leading_comments);
                    print_toml_comment(printer, fieldSourceLoc.trailing_comments);
                    printer->Print("\"### end\\n\");\n");
                }
                printer->Print(_variables, "result[\"$name$\"] = val;\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
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
                printer->Print(_variables, "arr.push_back($name$[i]);\n");
                printer->Outdent();
                printer->Print("}\n");
                printer->Print(_variables, "result[\"$name$\"] = arr;\n");
                break;
        }
    }

    std::string PrimitiveFieldGenerator::do_get_default_value(void) const {
        switch (descriptor_->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                return turbo::str_cat(descriptor_->default_value_int32());
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                return turbo::str_cat(descriptor_->default_value_int64()) + "ll";
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                return turbo::str_cat(descriptor_->default_value_uint32()) + "u";
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                return turbo::str_cat(descriptor_->default_value_uint64()) + "ull";
            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                return turbo::str_cat(descriptor_->default_value_float());
            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                return turbo::str_cat(descriptor_->default_value_double());
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                return descriptor_->default_value_bool() ? "1" : "0";
            default:
                KLOG(FATAL) << "unexpected CPPTYPE in c_primitive_field";
                return "UNEXPECTED_CPPTYPE";
        }
    }

    std::string PrimitiveFieldGenerator::get_default_value() const {
        return turbo::str_format("{%s}", do_get_default_value());
    }
} // namespace shark
