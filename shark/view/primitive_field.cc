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
#include <shark/view/primitive_field.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>

namespace shark {
    PrimitiveFieldViewGenerator::
    PrimitiveFieldViewGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldNoMetaGenerator(descriptor) {
        std::string c_type = get_ctype(descriptor_, descriptor_->containing_type());
        _variables["c_type"] = c_type;
        is_atomic = _ext_option.is_atomic();
    }

    PrimitiveFieldViewGenerator::~PrimitiveFieldViewGenerator() {
    }

    void PrimitiveFieldViewGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "shark::PrimitiveView<$c_type$> _$name$;\n");
    }

    void PrimitiveFieldViewGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$deprecated$inline shark::PrimitiveView<$c_type$> $name$() const;\n");
    }

    void PrimitiveFieldViewGenerator::generate_members_inline_implementations(
        google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "inline shark::PrimitiveView<$c_type$> $domain_view$::$name$$deprecated$() const {\n");
        printer->Indent();
        if (is_atomic) {
            printer->Print(_variables, "return _$name$.load();\n");
        } else {
            printer->Print(_variables, "return _$name$;\n");
        }
        printer->Outdent();
        printer->Print(_variables, "}\n");
    }

    void PrimitiveFieldViewGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:

                if (is_atomic) {
                    printer->Print(_variables, "_$name$.store(pb.$name$());\n");
                } else {
                    printer->Print(_variables, "_$name$ = pb.$name$();\n");
                }
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$.reserve(pb.$name$_size());\n");
                printer->Print(_variables, "auto &ref = pb.$name$();\n");
                printer->Print(_variables, "for(size_t i = 0; i < pb.$name$_size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "_$name$.push_back(ref[i]);\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }

    }

    void PrimitiveFieldViewGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "_$name$.store(rhs._$name$.load());\n");
                } else {
                    printer->Print(_variables, "_$name$ = rhs._$name$;\n");
                }

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$ = std::move(rhs._$name$);\n");
                break;
        }
    }
    void PrimitiveFieldViewGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "_$name$.store(rhs._$name$.load());\n");
                } else {
                    printer->Print(_variables, "_$name$ = rhs._$name$;\n");
                }

                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "_$name$ = rhs._$name$;\n");
                break;
        }
    }

    void PrimitiveFieldViewGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (is_atomic) {
                    printer->Print(_variables, "pb.set_$name$(_$name$.load());\n");
                } else {
                    printer->Print(_variables, "pb.set_$name$(_$name$);\n");
                }
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(_variables, "pb.mutable_$name$()->Reserve(_$name$.size());\n");
                printer->Print(_variables, "for(size_t i = 0; i < _$name$.size(); ++i) {\n");
                printer->Indent();
                printer->Print(_variables, "*pb.mutable_$name$()->Add() = _$name$[i];\n");
                printer->Outdent();
                printer->Print("}\n");
                break;
        }
    }

    std::string PrimitiveFieldViewGenerator::do_get_default_value(void) const {
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

    std::string PrimitiveFieldViewGenerator::get_default_value() const {
        return do_get_default_value();
    }



} // namespace shark
