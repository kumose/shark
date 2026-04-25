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

#include <shark/rt/oneof_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    // ===================================================================
    OneofFieldGenerator::OneofFieldGenerator(const google::protobuf::OneofDescriptor *_oneof) : OneofFieldGeneratorBase(
        _oneof) {
    }

    void OneofFieldGenerator::generate_enum_def(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "enum class $foneofname$Case {\n");
        printer->Indent();
        printer->Print(_variable, "NONE = 0,\n");
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            _variable["fieldname"] = CamelToUpper(field->name());
            _variable["fieldnum"] = turbo::str_cat(field->number());
            bool isLast = j == _oneof->field_count() - 1;
            if (isLast) {
                _variable["opt_comma"] = "";
            }
            printer->Print(_variable, "$fieldname$ = $fieldnum$$opt_comma$\n");
        }
        printer->Outdent();
        printer->Print(_variable, "};\n\n");
    }

    void OneofFieldGenerator::generate_members(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "union $foneofname$ {\n");
        printer->Indent();
        printer->Print(_variable, "int64_t _$foneofname$_placeholder{0};\n");
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["type"] = type;
            if (is_primitive_type(field->type())) {
                printer->Print(_variable, "$type$ _$fieldname$;\n");
            } else {
                printer->Print(_variable, "std::unique_ptr<$type$> _$fieldname$;\n");
            }
        }
        printer->Print(_variable, "$foneofname$() {}\n\n");
        printer->Print(_variable, "~$foneofname$() {}\n\n");
        printer->Outdent();
        printer->Print(_variable, "} _$foneofname$;\n\n");
        printer->Print(_variable, "$foneofname$Case _$foneofname$_case{$foneofname$Case::NONE};\n\n");
    }

    void OneofFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "inline $foneofname$Case $foneofname$_case() const;\n");
        printer->Print(_variable, "inline bool has_$foneofname$() const;\n");
        printer->Print(_variable, "inline void clear_$foneofname$();\n");
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["type"] = type;
            printer->Print(_variable, "inline bool has_$fieldname$() const;\n");
            printer->Print(_variable, "inline void set_$fieldname$($type$ val);\n");
            if (is_primitive_type(field->type())) {
                printer->Print(_variable, "inline $type$ $fieldname$() const;\n");
            } else {
                printer->Print(_variable, "inline const $type$& $fieldname$() const;\n");
            }
        }
    }

    void OneofFieldGenerator::generate_ctor_define(google::protobuf::io::Printer *printer) {
        _variable["foneofname"] = _oneof->name();
        printer->Print(_variable, "clear_$foneofname$(); \n");
    }

    void OneofFieldGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) {
        /// clear
        printer->Print(_variable, "clear_$foneofname$();\n");
        printer->Print(_variable, "switch(pb.$foneofname$_case()) {\n");
        printer->Indent();
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["pbenumname"] = turbo::str_cat("k", ToCamel(field->name()));
            _variable["enumname"] = CamelToUpper(field->name());

            _variable["type"] = type;
            printer->Print(_variable, "case $pb_uri$::$Coneofname$Case::$pbenumname$:\n");
            printer->Indent();

            if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = std::make_unique<$type$>();\n");
                printer->Print(_variable, "_$foneofname$._$fieldname$->parse_from_proto(pb.$fieldname$());\n");
            } else if (field->type() == google::protobuf::FieldDescriptor::TYPE_ENUM) {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = static_cast<$type$>(pb.$fieldname$());\n");
            } else if (!is_primitive_type(field->type())) {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = std::make_unique<$type$>();\n");
                printer->Print(
                    _variable,
                    "*_$foneofname$._$fieldname$ = pb.$fieldname$();\n");
            }  else {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = pb.$fieldname$();\n");
            }
            printer->Print(_variable, "_$foneofname$_case = $foneofname$Case::$enumname$;\n");
            printer->Print(_variable, "break;\n");
            printer->Outdent();
        }
        printer->Print(_variable, "default:\n");
        printer->Indent();
        printer->Print(_variable, "break;\n");
        printer->Outdent();
        printer->Outdent();
        printer->Print(_variable, "};\n");
    }

    void OneofFieldGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) {
        /// clear
        printer->Print(_variable, "switch($foneofname$_case()) {\n");
        printer->Indent();
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["enumname"] = CamelToUpper(field->name());
            _variable["type"] = type;
            printer->Print(_variable, "case $foneofname$Case::$enumname$:\n");
            printer->Indent();
            if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
                printer->Print(_variable, "_$foneofname$._$fieldname$->serialize_to_proto(*pb.mutable_$fieldname$());\n");
            }  else if (field->type() == google::protobuf::FieldDescriptor::TYPE_ENUM) {
                printer->Print(_variable, "pb.set_$fieldname$(static_cast<$pb_type$>(_$foneofname$._$fieldname$));\n");
            }else  {
                printer->Print(_variable, "pb.set_$fieldname$($fieldname$());\n");
            }
            printer->Print(_variable, "break;\n");
            printer->Outdent();
        }
        printer->Print(_variable, "default:\n");
        printer->Indent();
        printer->Print(_variable, "break;\n");
        printer->Outdent();
        printer->Outdent();
        printer->Print(_variable, "};\n");
    }

    void OneofFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) {
        /// clear
        printer->Print(_variable, "clear_$foneofname$();\n");
        printer->Print(_variable, "switch(rhs.$foneofname$_case()) {\n");
        printer->Indent();
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["enumname"] = CamelToUpper(field->name());
            _variable["type"] = type;
            printer->Print(_variable, "case $foneofname$Case::$enumname$:\n");
            printer->Indent();
            if (!is_primitive_type(field->type()) && field->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = std::make_unique<$type$>();\n");
                printer->Print(
                    _variable,
                    "*_$foneofname$._$fieldname$ = *rhs._$foneofname$._$fieldname$;\n");
            } else {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = rhs._$foneofname$._$fieldname$;\n");
            }
            printer->Print(_variable, "break;\n");
            printer->Outdent();
        }
        printer->Print(_variable, "default:\n");
        printer->Indent();
        printer->Print(_variable, "break;\n");
        printer->Outdent();
        printer->Outdent();
        printer->Print(_variable, "};\n");
        printer->Print(_variable, "_$foneofname$_case = rhs._$foneofname$_case;\n");
    }

    void OneofFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "clear_$foneofname$();\n");
        printer->Print(_variable, "switch(rhs.$foneofname$_case()) {\n");
        printer->Indent();
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            _variable["fieldname"] = field->name();
            _variable["enumname"] = CamelToUpper(field->name());
            _variable["type"] = type;
            printer->Print(_variable, "case $foneofname$Case::$enumname$:\n");
            printer->Indent();
            if (!is_primitive_type(field->type())) {
                printer->Print(
                    _variable,
                    "_$foneofname$._$fieldname$ = std::move(rhs._$foneofname$._$fieldname$);\n");
            } else {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = rhs._$foneofname$._$fieldname$;\n");
            }
            printer->Print(_variable, "break;\n");
            printer->Outdent();
        }
        printer->Print(_variable, "default:\n");
        printer->Indent();
        printer->Print(_variable, "break;\n");
        printer->Outdent();
        printer->Outdent();
        printer->Print(_variable, "};\n");
        printer->Print(_variable, "_$foneofname$_case = rhs._$foneofname$_case;\n");
    }

    void OneofFieldGenerator::generate_dtor_define(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "clear_$foneofname$(); \n");
    }

    void OneofFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) {
        printer->Print(_variable, "inline $domain$::$foneofname$Case $domain$::$foneofname$_case() const {\n");
        printer->Indent();
        printer->Print(_variable, "return _$foneofname$_case;\n");
        printer->Outdent();
        printer->Print(_variable, "}\n");

        printer->Print(_variable, "inline bool $domain$::has_$foneofname$() const {\n");
        printer->Indent();
        printer->Print(_variable, "return _$foneofname$_case != $foneofname$Case::NONE;\n");
        printer->Outdent();
        printer->Print(_variable, "}\n");
        {
            /// clear
            printer->Print(_variable, "inline void $domain$::clear_$foneofname$() {\n");
            printer->Indent();
            printer->Print(_variable, "switch(_$foneofname$_case) {\n");
            printer->Indent();
            for (int j = 0; j < _oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = _oneof->field(j);
                auto type = get_ctype(field, _descriptor);
                _variable["fieldname"] = field->name();
                _variable["enumname"] = CamelToUpper(field->name());
                _variable["type"] = type;
                printer->Print(_variable, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                if (!is_primitive_type(field->type())) {
                    printer->Print(_variable, "_$foneofname$._$fieldname$.reset();\n");
                }
                printer->Print(_variable, "break;\n");
                printer->Outdent();
            }
            printer->Print(_variable, "default:\n");
            printer->Indent();
            printer->Print(_variable, "break;\n");
            printer->Outdent();
            printer->Outdent();
            printer->Print(_variable, "};\n");
            printer->Print(_variable, "_$foneofname$_case = $foneofname$Case::NONE;\n");
            printer->Print(_variable, "_$foneofname$._$foneofname$_placeholder = 0;\n");
            printer->Outdent();
            printer->Print(_variable, "}\n\n");
        }
        for (int j = 0; j < _oneof->field_count(); j++) {
            const google::protobuf::FieldDescriptor *field = _oneof->field(j);
            auto type = get_ctype(field, _descriptor);
            auto g_type = get_ctype(field, nullptr);
            _variable["fieldname"] = field->name();
            _variable["enumname"] = CamelToUpper(field->name());
            _variable["type"] = type;
            _variable["g_type"] = g_type;
            printer->Print(_variable, "inline bool $domain$::has_$fieldname$() const {\n");
            printer->Indent();
            printer->Print(_variable, "return _$foneofname$_case == $foneofname$Case::$enumname$;\n");
            printer->Outdent();
            printer->Print(_variable, "}\n");
            printer->Print(_variable, "inline void $domain$::set_$fieldname$($type$ val) {\n");
            printer->Indent();
            printer->Print(_variable, "clear_$foneofname$();\n");
            if (is_primitive_type(field->type())) {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = val;\n");
            } else {
                printer->Print(_variable, "_$foneofname$._$fieldname$ = std::make_unique<$type$>(val);\n");
            }
            printer->Print(_variable, "_$foneofname$_case = $foneofname$Case::$enumname$;\n");
            printer->Outdent();
            printer->Print(_variable, "}\n");
            if (is_primitive_type(field->type())) {
                printer->Print(_variable, "inline $g_type$ $domain$::$fieldname$() const {\n");
            } else {
                printer->Print(_variable, "inline const $g_type$& $domain$::$fieldname$() const {\n");
            }
            printer->Indent();
            if (is_primitive_type(field->type())) {
                printer->Print(_variable, "return _$foneofname$._$fieldname$;\n");
            } else {
                printer->Print(_variable, "return *_$foneofname$._$fieldname$;\n");
            }

            printer->Outdent();
            printer->Print(_variable, "}\n");
        }
    }


    std::string OneofFieldGenerator::get_default_value(void) {
        return "";
    }
} // namespace shark
