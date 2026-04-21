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

#include <shark/generator/oneof_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/generator/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    // ===================================================================

    OneofFieldGenerator::~OneofFieldGenerator() {
    }

    void OneofFieldGenerator::generate_enum_def(google::protobuf::io::Printer *printer) const {
        // Generate the case enums for unions
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);
            _vars["opt_comma"] = ",";

            _vars["oneofname"] = CamelToUpper(oneof->name());
            _vars["foneofname"] = oneof->name();

            printer->Print(_vars, "enum class $foneofname$Case {\n");
            printer->Indent();
            printer->Print(_vars, "NONE = 0,\n");
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                _vars["fieldname"] = CamelToUpper(field->name());
                _vars["fieldnum"] = SimpleItoa(field->number());
                bool isLast = j == oneof->field_count() - 1;
                if (isLast) {
                    _vars["opt_comma"] = "";
                }
                printer->Print(_vars, "$fieldname$ = $fieldnum$$opt_comma$\n");
            }
            printer->Outdent();
            printer->Print(_vars, "};\n\n");
        }
    }

    void OneofFieldGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();

            printer->Print(_vars, "union $foneofname$ {\n");
            printer->Indent();
            printer->Print(_vars, "int64_t _$foneofname$_placeholder{0};\n");
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["type"] = type;
                printer->Print(_vars, "$type$ _$fieldname$;\n");
            }
            printer->Print(_vars, "$foneofname$() {};\n\n");
            printer->Print(_vars, "~$foneofname$() {};\n\n");
            printer->Outdent();
            printer->Print(_vars, "} _$foneofname$;\n\n");
            printer->Print(_vars, "$foneofname$Case _$foneofname$_case{$foneofname$Case::NONE};\n\n");
        }
    }

    void OneofFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();

            printer->Print(_vars, "inline $foneofname$Case $foneofname$_case() const;\n");
            printer->Print(_vars, "inline bool has_$foneofname$() const;\n");
            printer->Print(_vars, "inline void clear_$foneofname$();\n");
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["type"] = type;
                printer->Print(_vars, "inline bool has_$fieldname$() const;\n");
                printer->Print(_vars, "inline void set_$fieldname$($type$ val);\n");
                printer->Print(_vars, "inline std::optional<$type$> $fieldname$() const;\n");
            }
        }
    }

    void OneofFieldGenerator::generate_ctor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;
            printer->Print(_vars, "clear_$foneofname$(); \n");
        }
    }

    void OneofFieldGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        auto pb_uri = turbo::str_replace_all(mdes_->full_name(), {{".", "::"}});
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;
            _vars["pb_uri"] = pb_uri;
            _vars["Coneofname"] = ToCamel(oneof->name());
            /// clear
            printer->Print(_vars, "clear_$foneofname$();\n");
            printer->Print(_vars, "switch(pb.$foneofname$_case()) {\n");
            printer->Indent();
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["pbenumname"] = turbo::str_cat("k", ToCamel(field->name()));
                _vars["enumname"] = CamelToUpper(field->name());

                _vars["type"] = type;
                printer->Print(_vars, "case $pb_uri$::$Coneofname$Case::$pbenumname$:\n");
                printer->Indent();
                if (!GlobalState::is_primitive_type(field->type())) {
                    printer->Print(
                        _vars,
                        "new (&_$foneofname$._$fieldname$) decltype(_$foneofname$._$fieldname$)(pb.$fieldname$());\n");
                } else {
                    printer->Print(_vars, "_$foneofname$._$fieldname$ = pb.$fieldname$();\n");
                }
                printer->Print(_vars, "_$foneofname$_case = $foneofname$Case::$enumname$;\n");
                printer->Print(_vars, "break;\n");
                printer->Outdent();
            }
            printer->Print(_vars, "default:\n");
            printer->Indent();
            printer->Print(_vars, "break;\n");
            printer->Outdent();
            printer->Outdent();
            printer->Print(_vars, "};\n");
        }
    }

    void OneofFieldGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;

            /// clear
            printer->Print(_vars, "switch($foneofname$_case()) {\n");
            printer->Indent();
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                printer->Print(_vars, "pb.set_$fieldname$(_$foneofname$._$fieldname$);\n");
                printer->Print(_vars, "break;\n");
                printer->Outdent();
            }
            printer->Print(_vars, "default:\n");
            printer->Indent();
            printer->Print(_vars, "break;\n");
            printer->Outdent();
            printer->Outdent();
            printer->Print(_vars, "};\n");
        }
    }

    void OneofFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;

            /// clear
            printer->Print(_vars, "clear_$foneofname$();\n");
            printer->Print(_vars, "switch(rhs.$foneofname$_case()) {\n");
            printer->Indent();
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                if (!GlobalState::is_primitive_type(field->type())) {
                    printer->Print(
                        _vars,
                        "new (&_$foneofname$._$fieldname$) decltype(_$foneofname$._$fieldname$)(rhs._$foneofname$._$fieldname$);\n");
                } else {
                    printer->Print(_vars, "_$foneofname$._$fieldname$ = rhs._$foneofname$._$fieldname$;\n");
                }
                printer->Print(_vars, "break;\n");
                printer->Outdent();
            }
            printer->Print(_vars, "default:\n");
            printer->Indent();
            printer->Print(_vars, "break;\n");
            printer->Outdent();
            printer->Outdent();
            printer->Print(_vars, "};\n");
            printer->Print(_vars, "_$foneofname$_case = rhs._$foneofname$_case;\n");
        }
    }

    void OneofFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;

            /// clear
            printer->Print(_vars, "clear_$foneofname$();\n");
            printer->Print(_vars, "switch(rhs.$foneofname$_case()) {\n");
            printer->Indent();
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                if (!GlobalState::is_primitive_type(field->type())) {
                    printer->Print(
                        _vars,
                        "new (&_$foneofname$._$fieldname$) decltype(_$foneofname$._$fieldname$)(std::move(rhs._$foneofname$._$fieldname$));\n");
                } else {
                    printer->Print(_vars, "_$foneofname$._$fieldname$ = rhs._$foneofname$._$fieldname$;\n");
                }
                printer->Print(_vars, "break;\n");
                printer->Outdent();
            }
            printer->Print(_vars, "default:\n");
            printer->Indent();
            printer->Print(_vars, "break;\n");
            printer->Outdent();
            printer->Outdent();
            printer->Print(_vars, "};\n");
            printer->Print(_vars, "_$foneofname$_case = rhs._$foneofname$_case;\n");
        }
    }

    void OneofFieldGenerator::generate_dtor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;
            printer->Print(_vars, "clear_$foneofname$(); \n");
        }
    }

    void OneofFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;

            printer->Print(_vars, "inline $msg$::$foneofname$Case $msg$::$foneofname$_case() const {\n");
            printer->Indent();
            printer->Print(_vars, "return _$foneofname$_case;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n");

            printer->Print(_vars, "inline bool $msg$::has_$foneofname$() const {\n");
            printer->Indent();
            printer->Print(_vars, "return _$foneofname$_case != $foneofname$Case::NONE;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n");
            {
                /// clear
                printer->Print(_vars, "inline void $msg$::clear_$foneofname$() {\n");
                printer->Indent();
                printer->Print(_vars, "switch(_$foneofname$_case) {\n");
                printer->Indent();
                for (int j = 0; j < oneof->field_count(); j++) {
                    const google::protobuf::FieldDescriptor *field = oneof->field(j);
                    auto type = GlobalState::get_ctype(field);
                    _vars["fieldname"] = field->name();
                    _vars["enumname"] = CamelToUpper(field->name());
                    _vars["type"] = type;
                    printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                    printer->Indent();
                    if (!GlobalState::is_primitive_type(field->type())) {
                        printer->Print(_vars, "_$foneofname$._$fieldname$.~decltype(_$foneofname$._$fieldname$)();\n");
                    }
                    printer->Print(_vars, "break;\n");
                    printer->Outdent();
                }
                printer->Print(_vars, "default:\n");
                printer->Indent();
                printer->Print(_vars, "break;\n");
                printer->Outdent();
                printer->Outdent();
                printer->Print(_vars, "};\n");
                printer->Print(_vars, "_$foneofname$_case = $foneofname$Case::NONE;\n");
                printer->Print(_vars, "_$foneofname$._$foneofname$_placeholder = 0;\n");
                printer->Outdent();
                printer->Print(_vars, "}\n\n");
            }
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = GlobalState::get_ctype(field);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "inline bool $msg$::has_$fieldname$() const {\n");
                printer->Indent();
                printer->Print(_vars, "return _$foneofname$_case == $foneofname$Case::$enumname$;\n");
                printer->Outdent();
                printer->Print(_vars, "}\n");
                printer->Print(_vars, "inline void $msg$::set_$fieldname$($type$ val) {\n");
                printer->Indent();
                printer->Print(_vars, "clear_$foneofname$();\n");
                if (GlobalState::is_primitive_type(field->type())) {
                    printer->Print(_vars, "_$foneofname$._$fieldname$ = val;\n");
                } else {
                    printer->Print(_vars, "new (&_$foneofname$._$fieldname$) $type$(std::move(val));\n");
                }
                printer->Print(_vars, "_$foneofname$_case = $foneofname$Case::$enumname$;\n");
                printer->Outdent();
                printer->Print(_vars, "}\n");
                printer->Print(_vars, "inline std::optional<$type$> $msg$::$fieldname$() const {\n");
                printer->Indent();
                printer->Print(_vars, "if(_$foneofname$_case != $foneofname$Case::$enumname$) {\n");
                printer->Indent();
                printer->Print(_vars, "return std::nullopt;\n");
                printer->Outdent();
                printer->Print(_vars, "}\n");
                printer->Print(_vars, "return _$foneofname$._$fieldname$;\n");
                printer->Outdent();
                printer->Print(_vars, "}\n");
            }
        }
    }


    void OneofFieldGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
    }

    std::string OneofFieldGenerator::do_get_default_value(void) const {
        return "";
    }

    std::string OneofFieldGenerator::get_default_value(void) const {
        return do_get_default_value();
    }

    void OneofFieldGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {
    }

    void OneofFieldGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
