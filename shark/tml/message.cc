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

#include <shark/tml/enum.h>
#include <shark/tml/extension.h>
#include <shark/utility/helpers.h>
#include <shark/tml/message.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_replace.h>
#include <shark/utility/uri.h>

namespace shark {
    MessageGenerator::MessageGenerator(const google::protobuf::Descriptor *descriptor,
                                       const std::string &dllexport_decl)
        : MessageGeneratorBase(descriptor, dllexport_decl),
          field_generators_(descriptor) {
        GlobalState::instance().registry(_descriptor);
        for (int i = 0; i < descriptor->nested_type_count(); i++) {
            if (descriptor->nested_type(i)->options().map_entry()) {
                continue;
            }
            auto ptr = std::make_unique<MessageGenerator>(descriptor->nested_type(i), dllexport_decl);
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

        if (descriptor->oneof_decl_count() > 0) {
            KLOG(FATAL)<<"shark do not support oneof fields";
        }

        _vars["classname"] = _descriptor->name();
        _vars["domain"] = message_type(_descriptor);
        _vars["lcclassname"] = FullNameToLower(_descriptor->full_name(), _descriptor->file());
        _vars["ucclassname"] = FullNameToUpper(_descriptor->full_name(), _descriptor->file());
        _vars["field_count"] = turbo::str_cat(_descriptor->field_count());
        _vars["PBTYPE"] = turbo::str_replace_all(_descriptor->full_name(), {{".", "::"}});
        if (_dllexport_decl.empty()) {
            _vars["dllexport"] = "";
        } else {
            _vars["dllexport"] = _dllexport_decl + " ";
        }
    }

    MessageGenerator::~MessageGenerator() {
    }

    void MessageGenerator::
    generate_fwd_typedef(google::protobuf::io::Printer *printer) {
        printer->Print("class $classname$;\n",
                       "classname", _descriptor->name());
    }

    void MessageGenerator::generate_inline_implement(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_inline_implement(printer);
        }

        for (int i = 0; i < _descriptor->enum_type_count(); i++) {
            printer->Print("\n");
            enum_generators_[i]->generate_inline_definition(printer, _descriptor);
        }
    }


    void MessageGenerator::
    generate_enum_definitions(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < _descriptor->enum_type_count(); i++) {
            enum_generators_[i]->generate_definition(printer);
            printer->Print("\n");
            enum_generators_[i]->generate_inline_declarations(printer, _descriptor);
        }
    }


    void MessageGenerator::generate_definition(google::protobuf::io::Printer *printer) {
        printer->Print(_vars,
                       "class $dllexport$ $classname$ {\n"
                       "public:\n\n");
        printer->Print(_vars, "///  runtime identify for traits\n");
        printer->Print(_variables, "static constexpr bool is_runtime_type = true;\n\n");


        printer->Print(_vars,
                       "\npublic:\n");
        printer->Indent();

        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_definition(printer);
        }

        /// generate oneof definitiion


        google::protobuf::SourceLocation msgSourceLoc;
        _descriptor->GetSourceLocation(&msgSourceLoc);
        PrintComment(printer, msgSourceLoc.leading_comments);

        const idl::SharkMessageOptions opt =
                _descriptor->options().GetExtension(idl::shark_message);

        // Generate fields.
        printer->Print("/// -----enums-------- \n");
        generate_enum_definitions(printer);
        printer->Outdent();
        printer->Print("public:\n");
        printer->Indent();

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// transfers \n");
        if (_descriptor->containing_type() == nullptr) {
            printer->Print(_vars, "turbo::Status parse_toml_str(const std::string& path);\n\n");
            printer->Print(_vars, "turbo::Status parse_toml_file(const std::string& str);\n\n");
            printer->Print(_vars, "turbo::Status parse_toml(const xtoml::Value& v, const xtoml::TomlUri &prefix);\n\n");
            printer->Print(_vars, "void add_checker(std::unique_ptr<xtoml::BasicHandler> handler);\n\n");
            printer->Print(_vars, "const xtoml::HandlerMap &checkers() const;\n\n");
        } else {
            printer->Print(_vars, "turbo::Status parse_toml(const xtoml::Value& v, const xtoml::TomlUri &prefix, const xtoml::HandlerMap &map);\n\n");
        }
        printer->Print(_vars, "xtoml::Value serialize_toml() const;\n\n");
        printer->Print(_vars, "turbo::Result<std::string> serialize_to_string() const;\n\n");
        printer->Outdent();
        printer->Print("\n//////////////////////////////////////////////////////////////////////\n");
        printer->Print("/// members\n");
        printer->Print("/// Keep members protected for inheritance and extension\n");
        printer->Print("public:\n");
        printer->Indent();
        for (int i = 0; i < _descriptor->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = _descriptor->field(i);
            if (field->containing_oneof() == NULL) {
                google::protobuf::SourceLocation fieldSourceLoc;
                field->GetSourceLocation(&fieldSourceLoc);

                PrintComment(printer, fieldSourceLoc.leading_comments);
                PrintComment(printer, fieldSourceLoc.trailing_comments);
                field_generators_.get(field).generate_members(printer);
            }
        }

        printer->Outdent();
        if (_descriptor->containing_type() == nullptr) {
            printer->Print("\n//////////////////////////////////////////////////////////////////////\n");
            printer->Print("/// global checkers\n\n");
            printer->Print("xtoml::TomlUri    _uri_prefix;\n");
            printer->Print("xtoml::HandlerMap _handler_map;\n");
        }
        printer->Print(_vars, "};\n");
    }

    void MessageGenerator::generate_static_variable(google::protobuf::io::Printer *printer) {
    }

    void MessageGenerator::generate_static_functions(google::protobuf::io::Printer *printer) {
    }

    void MessageGenerator::generate_ctor_implemention(google::protobuf::io::Printer *printer) {

    }

    void MessageGenerator::generate_cpy_ctor_implemention(google::protobuf::io::Printer *printer) {

    }

    void MessageGenerator::generate_move_ctor_implement(google::protobuf::io::Printer *printer) {

    }

    void MessageGenerator::generate_trans_implement(google::protobuf::io::Printer *printer) {
        if (_descriptor->containing_type() == nullptr) {
            printer->Print(_vars, "turbo::Status $domain$::parse_toml_str(const std::string& str) {\n\n");
            printer->Indent();
            printer->Print(_vars, "try {\n");
            printer->Indent();
            printer->Print(_vars, "TURBO_MOVE_OR_RAISE(auto val , xtoml::parse_string(str));\n");
            printer->Print(_vars, "return  parse_toml(val, xtoml::TomlUri());\n");
            printer->Outdent();
            printer->Print(_vars, "} catch(const std::exception &e) {\n");
            printer->Indent();
            printer->Print(_vars, "return turbo::unknown_error(e.what());\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");

            printer->Print(_vars, "turbo::Status $domain$::parse_toml_file(const std::string& path) {\n");
            printer->Indent();
            printer->Print(_vars, "try {\n");
            printer->Indent();
            printer->Print(_vars, "TURBO_MOVE_OR_RAISE(auto val , xtoml::parse_file(path));\n");
            printer->Print(_vars, "return  parse_toml(val, xtoml::TomlUri());\n");
            printer->Outdent();
            printer->Print(_vars, "} catch(const std::exception &e) {\n");
            printer->Indent();
            printer->Print(_vars, "return turbo::unknown_error(e.what());\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");

            printer->Print(_vars, "void $domain$::add_checker(std::unique_ptr<xtoml::BasicHandler> handler) {\n");
            printer->Indent();
            printer->Print(_vars, "if(!handler) {\n");
            printer->Indent();
            printer->Print(_vars, "return;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");
            printer->Print(_vars, "_handler_map[handler->handler_uri()] = std::move(handler);\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");

            printer->Print(_vars, "const xtoml::HandlerMap& $domain$::checkers() const {\n\n");
            printer->Indent();
            printer->Print(_vars, "return _handler_map;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");
        }

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// transfers \n");
        if (_descriptor->containing_type() == nullptr) {
            printer->Print(_vars, "turbo::Status $domain$::parse_toml(const xtoml::Value& config, const xtoml::TomlUri &prefix) {\n");
            printer->Indent();
            printer->Print(_vars, "xtoml::TomlUri uri = prefix;\n");
            printer->Print(_vars, "const xtoml::HandlerMap &map = _handler_map;\n");
        } else {
            printer->Print(_vars, "turbo::Status $domain$::parse_toml(const xtoml::Value& config, const xtoml::TomlUri &prefix, const xtoml::HandlerMap &map) {\n\n");
            printer->Indent();
            printer->Print(_vars, "xtoml::TomlUri uri = prefix;\n");
        }


        for (int i = 0; i < _descriptor->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = _descriptor->field(i);
            if (field->containing_oneof() == NULL) {
                printer->Print(_vars, "{\n");
                printer->Indent();
                field_generators_.get(field).generate_trans_parse_toml_implementations(printer);
                printer->Outdent();
                printer->Print(_vars, "}\n");
            }
        }

        printer->Outdent();
        printer->Print("return turbo::OkStatus();\n");
        printer->Print("}\n\n");
        ////
        printer->Print(_vars, "xtoml::Value $domain$::serialize_toml() const {\n");
        printer->Indent();
        printer->Print(_vars, "xtoml::Value result;\n");
        for (int i = 0; i < _descriptor->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = _descriptor->field(i);
            if (field->containing_oneof() == NULL) {
                printer->Print(_vars, "{\n");
                printer->Indent();
                field_generators_.get(field).generate_trans_toml_implementations(printer);
                printer->Outdent();
                printer->Print(_vars, "}\n");
            }
        }
        printer->Print(_vars, "return result;\n");
        printer->Outdent();
        printer->Print("}\n\n");

        printer->Print(_vars, "turbo::Result<std::string> $domain$::serialize_to_string() const {\n\n");
        printer->Indent();
        printer->Print(_vars, "auto v = serialize_toml();\n\n");
        printer->Print(_vars, "return xtoml::serialize(v);\n\n");
        printer->Outdent();
        printer->Print("}\n\n");
    }

    void MessageGenerator::generate_implement(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < enum_generators_.size(); i++) {
            enum_generators_[i]->generate_implement(printer, _descriptor);
        }
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_implement(printer);
        }


        generate_ctor_implemention(printer);

        generate_cpy_ctor_implemention(printer);
        generate_move_ctor_implement(printer);

        generate_trans_implement(printer);
    }
} // namespace shark
