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



#include <algorithm>
#include <map>
#include <string_view>
#include <tuple>
#include <vector>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>

#include <shark/rt/enum.h>
#include <shark/rt/extension.h>
#include <shark/utility/helpers.h>
#include <shark/rt/message.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_replace.h>


namespace shark {
    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor *descriptor,
                                       const std::string &dllexport_decl, const MessageGenerator *parent)
        : descriptor_(descriptor),
          _dllexport_decl(dllexport_decl),
          field_generators_(descriptor),
            _oneof_generator(descriptor_){
        GlobalState::instance().registry(descriptor_);
        for (int i = 0; i < descriptor->nested_type_count(); i++) {
            if (descriptor->nested_type(i)->options().map_entry()) {
                continue;
            }
            auto ptr = std::make_unique<MessageGenerator>(descriptor->nested_type(i), dllexport_decl, this);
            nested_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->enum_type_count(); i++) {
            auto ptr = std::make_unique<EnumGenerator>(descriptor->enum_type(i), dllexport_decl);
            enum_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->extension_count(); i++) {
            auto ptr = std::make_unique<ExtensionGenerator>(descriptor->extension(i), dllexport_decl);
            extension_generators_.push_back(std::move(ptr));
        }
        _vars["classname"] = descriptor_->name();
        _vars["domain"] = message_type(descriptor_);
        _vars["lcclassname"] = FullNameToLower(descriptor_->full_name(), descriptor_->file());
        _vars["ucclassname"] = FullNameToUpper(descriptor_->full_name(), descriptor_->file());
        _vars["field_count"] = turbo::str_cat(descriptor_->field_count());
        _vars["PBTYPE"] = turbo::str_replace_all(descriptor_->full_name(), {{".", "::"}});
        if (_dllexport_decl.empty()) {
            _vars["dllexport"] = "";
        } else {
            _vars["dllexport"] = _dllexport_decl + " ";
        }
    }

    MessageGenerator::~MessageGenerator() {
    }

    void MessageGenerator::
    generate_struct_typedef(google::protobuf::io::Printer *printer) {
        printer->Print("class $classname$;\n",
                       "classname", descriptor_->name());
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_typedef(printer);
        }
    }

    void MessageGenerator::generate_struct_inl(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_inl(printer);
        }

        printer->Print(_vars,"inline const google::protobuf::Descriptor* $domain$::descriptor() const {\n");
        printer->Indent();
        printer->Print(_vars,"return get_descriptor();\n");
        printer->Outdent();
        printer->Print(_vars,"}\n\n");

        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                google::protobuf::SourceLocation fieldSourceLoc;
                field->GetSourceLocation(&fieldSourceLoc);

                PrintComment(printer, fieldSourceLoc.leading_comments);
                PrintComment(printer, fieldSourceLoc.trailing_comments);
                field_generators_.get(field).generate_members_inline_implementations(printer);
            }
        }
        _oneof_generator.generate_members_inline_implementations(printer);

        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            printer->Print("\n");
            enum_generators_[i]->generate_inline_definition(printer, descriptor_);
        }
    }


    void MessageGenerator::
    generate_enum_definitions(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            enum_generators_[i]->generate_definition(printer);
            printer->Print("\n");
            enum_generators_[i]->generate_inline_declarations(printer, descriptor_);
        }
    }


    void MessageGenerator::generate_struct_definition(google::protobuf::io::Printer *printer) {


        printer->Print(_vars,
                       "class $dllexport$ $classname$ {\n"
                       "public:\n");
        printer->Indent();
        printer->Print(_vars,"/// constructor\n");
        printer->Print(_vars,"$classname$();\n\n");
        printer->Print(_vars,"/// destructor\n");
        printer->Print(_vars,"~$classname$();\n\n");

        printer->Print(_vars,"$classname$(const $classname$& rhs);\n\n");
        printer->Print(_vars,"$classname$& operator= (const $classname$& rhs);\n\n");

        printer->Print(_vars,"$classname$($classname$&& rhs) noexcept;\n\n");
        printer->Print(_vars,"$classname$& operator= ($classname$&& rhs) noexcept;\n\n");

        printer->Print("/////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// meta describe \n");
        printer->Print(_vars,"static const google::protobuf::Descriptor* get_descriptor();\n\n");
        printer->Print(_vars,"const google::protobuf::Descriptor *descriptor() const;\n\n");

        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_definition(printer);
        }

        /// generate oneof definitiion
        _oneof_generator.generate_enum_def(printer);

        google::protobuf::SourceLocation msgSourceLoc;
        descriptor_->GetSourceLocation(&msgSourceLoc);
        PrintComment(printer, msgSourceLoc.leading_comments);

        const idl::SharkMessageOptions opt =
                descriptor_->options().GetExtension(idl::shark_message);

        // Generate fields.
        printer->Print("/// -----enums-------- \n");
        generate_enum_definitions(printer);
        printer->Outdent();
        printer->Print("public:\n");
        printer->Indent();
        printer->Print("////////////////////// getters/setters\n");
        printer->Print("\n");
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                google::protobuf::SourceLocation fieldSourceLoc;
                field->GetSourceLocation(&fieldSourceLoc);

                PrintComment(printer, fieldSourceLoc.leading_comments);
                PrintComment(printer, fieldSourceLoc.trailing_comments);
                field_generators_.get(field).generate_members_declares(printer);
            }
        }
        _oneof_generator.generate_members_declares(printer);
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// transfers \n");
        printer->Print(_vars, "void parse_from_proto(const $PBTYPE$& pb);\n\n");
        printer->Print(_vars, "void serialize_to_proto($PBTYPE$& pb) const;\n\n");
        printer->Print(_vars, "bool parse_from_json(const std::string& json);\n\n");
        printer->Print(_vars, "bool serialize_to_json(std::string& json) const;\n\n");
        printer->Print(_vars, "std::string to_string() const;\n\n");
        printer->Outdent();
        printer->Print("private:\n");
        printer->Indent();
        printer->Print("////////////////////// members\n");
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                google::protobuf::SourceLocation fieldSourceLoc;
                field->GetSourceLocation(&fieldSourceLoc);

                PrintComment(printer, fieldSourceLoc.leading_comments);
                PrintComment(printer, fieldSourceLoc.trailing_comments);
                field_generators_.get(field).generate_members(printer);
            }
        }
        printer->Print("////////////////////// unions\n");
        _oneof_generator.generate_members(printer);

        printer->Outdent();

        printer->Print(_vars, "};\n");
    }

    void MessageGenerator::generate_struct_transfer(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_transfer(printer);
        }
        printer->Print(_vars, "$domain$::$classname$() {\n");
        printer->Indent();
        _oneof_generator.generate_ctor_define(printer);
        printer->Outdent();
        printer->Print(_vars, "}\n\n");
        printer->Print(_vars, "$domain$::~$classname$() {\n");
        printer->Indent();
        _oneof_generator.generate_dtor_define(printer);
        printer->Outdent();
        printer->Print(_vars, "}\n\n");


        printer->Print(_vars,"$domain$::$classname$(const $classname$& rhs) {\n");
        printer->Indent();
        _oneof_generator.generate_copy_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_copy_ctor_define(printer);
            }
        }

        printer->Outdent();
        printer->Print(_vars, "}\n\n");
        printer->Print(_vars,"$domain$& $domain$::operator= (const $classname$& rhs) {\n");
        printer->Indent();
        _oneof_generator.generate_copy_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_copy_ctor_define(printer);
            }
        }

        printer->Print(_vars,"return *this;\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        printer->Print(_vars,"$domain$::$classname$($classname$&& rhs) noexcept {\n");
        printer->Indent();
        _oneof_generator.generate_move_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_move_ctor_define(printer);
            }
        }

        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        printer->Print(_vars,"$domain$& $domain$::operator= ($classname$&& rhs) noexcept {\n");
        printer->Indent();
        _oneof_generator.generate_move_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_move_ctor_define(printer);
            }
        }
        printer->Print(_vars,"return *this;\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// metas \n");
        printer->Print(_vars,"const google::protobuf::Descriptor* $domain$::get_descriptor() {\n");
        printer->Indent();
        printer->Print(_vars, "return $PBTYPE$::descriptor();\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// transfers \n");
        printer->Print(_vars, "void $domain$::parse_from_proto(const $PBTYPE$& pb) {\n");
        printer->Indent();
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_trans_parse_pb_implementations(printer);
            }
        }
        _oneof_generator.generate_trans_parse_pb_implementations(printer);
        printer->Outdent();
        printer->Print("}\n\n");
        ////
        printer->Print(_vars, "void $domain$::serialize_to_proto($PBTYPE$& pb) const {\n");
        printer->Indent();
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_trans_to_pb_implementations(printer);
            }
        }
        _oneof_generator.generate_trans_to_pb_implementations(printer);
        printer->Outdent();
        printer->Print("}\n\n");
        ////
        printer->Print(_vars, "bool $domain$::parse_from_json(const std::string& json) {\n");
        printer->Indent();
        printer->Print(_vars, "$PBTYPE$ pb;\n");
        printer->Print(_vars, "if(!google::protobuf::json::JsonStringToMessage(json, &pb, google::protobuf::json::ParseOptions()).ok()) {\n");
        printer->Indent();
        printer->Print(_vars, "return false;\n");
        printer->Outdent();
        printer->Print("}\n\n");
        printer->Print(_vars, "parse_from_proto(pb);\n");
        printer->Print(_vars, "return true;\n");
        printer->Outdent();
        printer->Print("}\n\n");
        ////
        printer->Print(_vars, "bool $domain$::serialize_to_json(std::string& json) const {\n");
        printer->Indent();
        printer->Print(_vars, "$PBTYPE$ pb;\n");
        printer->Print(_vars, "serialize_to_proto(pb);\n");
        printer->Print(_vars, "if(!google::protobuf::json::MessageToJsonString(pb, &json).ok()) {\n");
        printer->Indent();
        printer->Print(_vars, "return false;\n");
        printer->Outdent();
        printer->Print("}\n\n");
        printer->Print(_vars, "return true;\n");
        printer->Outdent();
        printer->Print("}\n\n");
        printer->Print(_vars, "std::string $domain$::to_string() const {\n");
        printer->Indent();
        printer->Print(_vars, "std::string json;\n");
        printer->Print(_vars, "auto b = serialize_to_json(json);\n");
        printer->Print(_vars, "if(b) {\n");
        printer->Indent();
        printer->Print(_vars, "return json;\n");
        printer->Outdent();
        printer->Print("}\n\n");
        printer->Print(_vars, "return \"\";\n");
        printer->Outdent();
        printer->Print("}\n\n");
    }

    void MessageGenerator::
    GenerateMessageDescriptor(google::protobuf::io::Printer *printer) {
    }
} // namespace shark
