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

#include <shark/builder/enum.h>
#include <shark/builder/extension.h>
#include <shark/utility/helpers.h>
#include <shark/builder/message.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_replace.h>


namespace shark {
    MessageSkbGenerator::MessageSkbGenerator(const google::protobuf::Descriptor *descriptor,
                                             const std::string &dllexport_decl, const MessageSkbGenerator *parent)
        : descriptor_(descriptor),
          dllexport_decl_(dllexport_decl),
          field_generators_(descriptor),
          _oneof_generator(descriptor_), _parent(parent) {
        GlobalState::instance().registry(descriptor_);
        for (int i = 0; i < descriptor->nested_type_count(); i++) {
            if (descriptor->nested_type(i)->options().map_entry()) {
                continue;
            }
            auto ptr = std::make_unique<MessageSkbGenerator>(descriptor->nested_type(i), dllexport_decl, this);
            nested_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->enum_type_count(); i++) {
            auto ptr = std::make_unique<EnumSkbGenerator>(descriptor->enum_type(i), dllexport_decl);
            enum_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->extension_count(); i++) {
            auto ptr = std::make_unique<ExtensionSkbGenerator>(descriptor->extension(i), dllexport_decl);
            extension_generators_.push_back(std::move(ptr));
        }
        _vars["classname"] = descriptor_->name() + "Builder";
        _vars["domain"] = message_domain_without_namespace(descriptor_, "Builder");
        _vars["rt_name"] = descriptor_->name();
        _vars["field_count"] = turbo::str_cat(descriptor_->field_count());
        _vars["PBTYPE"] = turbo::str_replace_all(descriptor_->full_name(), {{".", "::"}});
        if (dllexport_decl_.empty()) {
            _vars["dllexport"] = "";
        } else {
            _vars["dllexport"] = dllexport_decl_ + " ";
        }
        _shark_message_options = descriptor_->options().GetExtension(idl::shark_message);

        const google::protobuf::FieldDescriptor *field = _shark_message_options.GetDescriptor()->FindFieldByName(
            "skb_format");
        _vars["format_type"] = enum_type(field);
        _vars["format_value"] = enum_type(field) + "::" + idl::SharkFormat_Name(_shark_message_options.skb_format());
    }

    MessageSkbGenerator::~MessageSkbGenerator() {
    }

    void MessageSkbGenerator::
    generate_struct_typedef(google::protobuf::io::Printer *printer) {
        printer->Print(_vars, "class $classname$;\n");
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_typedef(printer);
        }
    }

    void MessageSkbGenerator::generate_struct_inl(google::protobuf::io::Printer *printer) {
        if (_parent) {
            return;
        }
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_inl(printer);
        }

        printer->Print(_vars, "inline const google::protobuf::Descriptor* $domain$::descriptor() const {\n");
        printer->Indent();
        printer->Print(_vars, "return get_descriptor();\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            printer->Print("\n");
            enum_generators_[i]->generate_inline_definition(printer, descriptor_);
        }
    }


    void MessageSkbGenerator::
    generate_enum_definitions(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            enum_generators_[i]->generate_definition(printer, descriptor_);
            printer->Print("\n");
            enum_generators_[i]->generate_inline_declarations(printer, descriptor_);
        }
    }


    void MessageSkbGenerator::generate_struct_definition(google::protobuf::io::Printer *printer) {
        if (_parent) {
            return;
        }
        printer->Print(_vars,
                       "class $dllexport$ $classname$ {\n"
                       "public:\n");
        printer->Indent();
        printer->Print(_vars, "/// constructor\n");
        printer->Print(_vars, "$classname$();\n\n");
        printer->Print(_vars, "/// destructor\n");
        printer->Print(_vars, "~$classname$();\n\n");

        printer->Print(_vars, "$classname$(const $classname$& rhs) = delete;\n\n");
        printer->Print(_vars, "$classname$& operator= (const $classname$& rhs) = delete;\n\n");

        printer->Print(_vars, "$classname$($classname$&& rhs) noexcept = default;\n\n");
        printer->Print(_vars, "$classname$& operator= ($classname$&& rhs) noexcept = default;\n\n");

        printer->Print("/////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// meta describe \n");
        printer->Print(_vars, "static const google::protobuf::Descriptor* get_descriptor();\n\n");
        printer->Print(_vars, "const google::protobuf::Descriptor *descriptor() const;\n\n");

        printer->Print("/////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// static default options region \n");

        printer->Print(_vars, "static constexpr $format_type$ kDefaultFormat = $format_value$;\n\n");

        printer->Print("/////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// using PtrContainer = std::variant<>\n");
        printer->Print(_vars, "using PtrContainer = std::variant<$PBTYPE$,$rt_name$>;\n");


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
        _oneof_generator.generate_members_declares(printer);
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// @name Appenders\n");
        printer->Print("/// @{\n\n");

        printer->Print(_vars,
                       "/// Appends a protobuf message via pointer.\n"
                       "/// @param pb Pointer to the protobuf message object. Must not be null.\n"
                       "/// @return Reference to this builder for method chaining.\n"
                       "/// @warning The caller must guarantee that the pointed-to object remains valid\n"
                       "///          until the builder is destroyed or `build()` is called. The builder\n"
                       "///          does not take ownership and will not delete the pointer.\n");
        printer->Print(_vars, "$classname$& append(const $PBTYPE$* pb);\n\n");

        printer->Print(_vars,
                       "/// Appends a runtime message via pointer.\n"
                       "/// @param rt Pointer to the runtime message object. Must not be null.\n"
                       "/// @return Reference to this builder for method chaining.\n"
                       "/// @warning The caller must ensure the pointed-to object outlives this builder\n"
                       "///          and any call to `build()`. No ownership is transferred.\n");
        printer->Print(_vars, "$classname$& append(const $rt_name$* rt);\n\n");

        printer->Print(_vars,
                       "/// Appends multiple messages from a vector of value objects.\n"
                       "/// @param rt Vector of messages (by value). The builder will copy/move each element\n"
                       "///           as needed; no external lifetime management is required.\n"
                       "/// @return Reference to this builder for method chaining.\n");
        printer->Print(_vars, "$classname$& append(const std::vector<PtrContainer>& rt);\n\n");

        printer->Print("/// @}\n\n");

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// builder \n");
        printer->Print(_vars,
                       "/// Serializes all appended messages into the output buffer.\n"
                       "/// @param result Output buffer. If `append` is false, buffer is cleared first.\n"
                       "/// @param error If non-null, set to a descriptive error message on failure.\n"
                       "/// @param append If true, append to existing buffer; otherwise overwrite.\n"
                       "/// @return true on success, false on error (e.g., invalid message).\n");
        printer->Print(_vars,
                       "bool build(std::vector<uint8_t>& result, std::string* error = nullptr, bool append = false) const;\n\n");

        printer->Outdent();
        printer->Print("private:\n");
        printer->Indent();

        printer->Print("////////////////////// members\n");
        printer->Print("std::vector<PtrContainer> _messages;\n");

        printer->Print("////////////////////// unions\n");
        _oneof_generator.generate_members(printer);

        printer->Outdent();

        printer->Print(_vars, "};\n");
    }

    void MessageSkbGenerator::generate_struct_transfer(google::protobuf::io::Printer *printer) {
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


        printer->Print(_vars, "$domain$::$classname$(const $classname$& rhs) {\n");
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
        printer->Print(_vars, "$domain$& $domain$::operator= (const $classname$& rhs) {\n");
        printer->Indent();
        _oneof_generator.generate_copy_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_copy_ctor_define(printer);
            }
        }

        printer->Print(_vars, "return *this;\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        printer->Print(_vars, "$domain$::$classname$($classname$&& rhs) noexcept {\n");
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

        printer->Print(_vars, "$domain$& $domain$::operator= ($classname$&& rhs) noexcept {\n");
        printer->Indent();
        _oneof_generator.generate_move_ctor_define(printer);
        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                field_generators_.get(field).generate_move_ctor_define(printer);
            }
        }
        printer->Print(_vars, "return *this;\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// metas \n");
        printer->Print(_vars, "const google::protobuf::Descriptor* $domain$::get_descriptor() {\n");
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
        printer->Print(
            _vars,
            "if(!google::protobuf::json::JsonStringToMessage(json, &pb, google::protobuf::json::ParseOptions()).ok()) {\n");
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

    void MessageSkbGenerator::
    GenerateMessageDescriptor(google::protobuf::io::Printer *printer) {
    }
} // namespace shark
