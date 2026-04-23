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

#include <shark/skb/view/oneof_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    // ===================================================================

    OneofFieldViewGenerator::~OneofFieldViewGenerator() {
    }

    void OneofFieldViewGenerator::generate_enum_def(google::protobuf::io::Printer *printer) const {
        // Generate the case enums for unions

        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);
            std::map<std::string, std::string> vars;
            vars["shortname"] = std::string(oneof->name());
            vars["domain"] = message_type(mdes_);

            printer->Print(vars, "using $shortname$Case = $domain$::$shortname$Case;\n\n");
        }

        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);
            _vars["shortname"] = std::string(oneof->name());
            _vars["domain"] = message_domain_without_namespace(mdes_);
            _vars["foneofname"] = oneof->name();
            _vars["soneofname"] = ToCamel(oneof->name());

            printer->Print(_vars, "struct $soneofname$ {\n");
            printer->Indent();
            printer->Print(_vars, "shark::EnumView<$domain$::$shortname$Case> $foneofname$_case;\n");
            printer->Print(_vars, "union Data {\n");
            printer->Indent();
            for (int j = 0; j < oneof->field_count(); j++) {
                const google::protobuf::FieldDescriptor *field = oneof->field(j);
                auto type = get_field_view(field);
                _vars["fieldname"] = field->name();
                _vars["type"] = type;
                printer->Print(_vars, "$type$ $fieldname$;\n");
            }
            printer->Print(_vars, "Data() {};\n\n");
            printer->Print(_vars, "~Data() {};\n\n");
            printer->Outdent();
            printer->Print(_vars, "} data;\n");
            printer->Outdent();
            printer->Print(_vars, "};\n\n");
        }
    }


    std::string OneofFieldViewGenerator::get_field_view(const google::protobuf::FieldDescriptor *field) const {
        const idl::SharkFieldOptions opt = field->options().GetExtension(idl::shark_field);
        switch (field->type()) {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                   KLOG(FATAL)<<"one not allow";
                }
                if (is_protobuf_any(field)) {
                    return "shark::AnyView";
                }
                return "shark::MessageView<" +message_domain_without_namespace(field->message_type(), "View") + ">";

            case google::protobuf::FieldDescriptor::TYPE_STRING:
                if (opt.string_as_bytes())
                    return "shark::BytesView";
                else
                    return "shark::StringView";
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return "shark::BytesView";
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
                return"shark::EnumView";
            default:
                std::string c_type = get_ctype(field, mdes_);
                return "shark::PrimitiveView<" + c_type + ">";
        }
    }

    void OneofFieldViewGenerator::generate_members(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["soneofname"] = ToCamel(oneof->name());
            printer->Print(_vars, "shark::OneOfView<$soneofname$> _$foneofname$;\n\n");
        }
    }

    void OneofFieldViewGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {

            std::map<std::string, std::string> _vars;
            for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
                const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

                _vars["foneofname"] = oneof->name();
                _vars["soneofname"] = ToCamel(oneof->name());
                printer->Print(_vars, "inline shark::OneOfView<$soneofname$> $foneofname$() const;\n");
            }
    }

    void OneofFieldViewGenerator::generate_ctor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;
            printer->Print(_vars, "clear_$foneofname$(); \n");
        }
    }

    void OneofFieldViewGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
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
                auto type = get_ctype(field, mdes_);
                _vars["fieldname"] = field->name();
                _vars["pbenumname"] = turbo::str_cat("k", ToCamel(field->name()));
                _vars["enumname"] = CamelToUpper(field->name());

                _vars["type"] = type;
                printer->Print(_vars, "case $pb_uri$::$Coneofname$Case::$pbenumname$:\n");
                printer->Indent();
                if (!is_primitive_type(field->type())) {
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

    void OneofFieldViewGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
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
                auto type = get_ctype(field, mdes_);
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

    void OneofFieldViewGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
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
                auto type = get_ctype(field, mdes_);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                if (!is_primitive_type(field->type())) {
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

    void OneofFieldViewGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
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
                auto type = get_ctype(field, mdes_);
                _vars["fieldname"] = field->name();
                _vars["enumname"] = CamelToUpper(field->name());
                _vars["type"] = type;
                printer->Print(_vars, "case $foneofname$Case::$enumname$:\n");
                printer->Indent();
                if (!is_primitive_type(field->type())) {
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

    void OneofFieldViewGenerator::generate_dtor_define(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["msg"] = msg_;
            printer->Print(_vars, "clear_$foneofname$(); \n");
        }
    }

    void OneofFieldViewGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        std::map<std::string, std::string> _vars;
        for (int i = 0; i < mdes_->oneof_decl_count(); i++) {
            const google::protobuf::OneofDescriptor *oneof = mdes_->oneof_decl(i);

            _vars["foneofname"] = oneof->name();
            _vars["soneofname"] = ToCamel(oneof->name());
            _vars["msg"] = msg_;
            printer->Print(_vars, "inline shark::OneOfView<$msg$::$soneofname$> $msg$::$foneofname$() const {;\n");
            printer->Indent();
            printer->Print(_vars, "return _$foneofname$;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n");
        }
    }


    void OneofFieldViewGenerator::GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
    }

    std::string OneofFieldViewGenerator::do_get_default_value(void) const {
        return "";
    }

    std::string OneofFieldViewGenerator::get_default_value(void) const {
        return do_get_default_value();
    }

    void OneofFieldViewGenerator::GenerateStaticInit(google::protobuf::io::Printer *printer) const {
    }

    void OneofFieldViewGenerator::GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const {
    }
} // namespace shark
