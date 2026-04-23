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

#include <shark/view/enum.h>
#include <shark/view/extension.h>
#include <shark/utility/helpers.h>
#include <shark/view/message.h>
#include <shark/utility/compat.h>
#include <shark/generator/global_state.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_replace.h>


namespace shark {
    MessageViewGenerator::MessageViewGenerator(const google::protobuf::Descriptor *descriptor,
                                               const std::string &dllexport_decl, const MessageViewGenerator *parent)
        : descriptor_(descriptor),
          dllexport_decl_(dllexport_decl),
          field_generators_(descriptor),
          _oneof_generator(descriptor_),
          _parent(parent) {
        GlobalState::instance().registry(descriptor_);
        for (int i = 0; i < descriptor->nested_type_count(); i++) {
            if (descriptor->nested_type(i)->options().map_entry()) {
                continue;
            }
            auto ptr = std::make_unique<MessageViewGenerator>(descriptor->nested_type(i), dllexport_decl, this);
            nested_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->enum_type_count(); i++) {
            auto ptr = std::make_unique<EnumViewGenerator>(descriptor->enum_type(i), dllexport_decl);
            enum_generators_.push_back(std::move(ptr));
        }

        for (int i = 0; i < descriptor->extension_count(); i++) {
            auto ptr = std::make_unique<ExtensionViewGenerator>(descriptor->extension(i), dllexport_decl);
            extension_generators_.push_back(std::move(ptr));
        }
        _vars["classname"] = descriptor_->name() + "View";
        _vars["domain"] = message_domain_without_namespace(descriptor_, "View");
        _vars["rt_name"] = descriptor_->name();
        _vars["lcclassname"] = FullNameToLower(descriptor_->full_name(), descriptor_->file());
        _vars["ucclassname"] = FullNameToUpper(descriptor_->full_name(), descriptor_->file());
        _vars["field_count"] = turbo::str_cat(descriptor_->field_count());
        _vars["PBTYPE"] = turbo::str_replace_all(descriptor_->full_name(), {{".", "::"}});
        if (dllexport_decl_.empty()) {
            _vars["dllexport"] = "";
        } else {
            _vars["dllexport"] = dllexport_decl_ + " ";
        }
    }

    MessageViewGenerator::~MessageViewGenerator() {
    }

    void MessageViewGenerator::
    generate_struct_typedef(google::protobuf::io::Printer *printer) {
        printer->Print(_vars, "class $classname$;\n");
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_typedef(printer);
        }
    }

    void MessageViewGenerator::generate_struct_inl(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_inl(printer);
        }

        printer->Print(_vars, "inline const google::protobuf::Descriptor* $domain$::descriptor() const {\n");
        printer->Indent();
        printer->Print(_vars, "return get_descriptor();\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        for (int i = 0; i < descriptor_->field_count(); i++) {
            const google::protobuf::FieldDescriptor *field = descriptor_->field(i);
            if (field->containing_oneof() == NULL) {
                google::protobuf::SourceLocation fieldSourceLoc;
                field->GetSourceLocation(&fieldSourceLoc);

                PrintComment(printer, fieldSourceLoc.leading_comments);
                PrintComment(printer, fieldSourceLoc.trailing_comments);
                field_generators_.get(field).generate_members_inline_implementations(printer);
            }
            field_generators_.get(field).generate_meta_definition_inline(printer);
        }
        _oneof_generator.generate_members_inline_implementations(printer);

        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            printer->Print("\n");
            enum_generators_[i]->generate_inline_definition(printer, descriptor_);
        }

        if (!_parent) {
        }
    }


    void MessageViewGenerator::
    generate_enum_definitions(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < descriptor_->enum_type_count(); i++) {
            enum_generators_[i]->generate_definition(printer, descriptor_);
            printer->Print("\n");
            enum_generators_[i]->generate_inline_declarations(printer, descriptor_);
        }
    }


    void MessageViewGenerator::generate_struct_definition(google::protobuf::io::Printer *printer) {
        printer->Print(_vars,
                       "class $dllexport$ $classname$ {\n"
                       "public:\n");
        printer->Indent();
        printer->Print(_vars, "/// constructor\n");
        printer->Print(_vars, "$classname$() = default;\n\n");
        printer->Print(_vars, "/// destructor\n");
        printer->Print(_vars, "~$classname$() = default;\n\n");

        printer->Print(_vars, "$classname$(const $classname$& rhs) = default;\n\n");
        printer->Print(_vars, "$classname$& operator= (const $classname$& rhs) = default;\n\n");

        printer->Print(_vars, "$classname$($classname$&& rhs) noexcept = default;\n\n");
        printer->Print(_vars, "$classname$& operator= ($classname$&& rhs) noexcept = default;\n\n");

        printer->Print("/////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// meta describe \n");
        printer->Print(_vars, "static const google::protobuf::Descriptor* get_descriptor();\n\n");
        printer->Print(_vars, "const google::protobuf::Descriptor *descriptor() const;\n\n");

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
        printer->Print("////////////////////// getters\n");
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
            field_generators_.get(field).generate_meta_declares(printer);
        }
        _oneof_generator.generate_members_declares(printer);

        if (!_parent) {
            printer->Print("///////////////////////////////////////////////////////////////////////// \n");
            printer->Print("/// serialize meta \n");


            printer->Print("/// Metadata for a single field (leaf or internal node).\n");
            printer->Print("/// \n");
            printer->Print("/// This structure captures all information needed to map a protobuf field\n");
            printer->Print("/// to a physical column in the zero-copy columnar layout.\n");
            printer->Print("/// For non-storage nodes (e.g., messages, maps, repeated containers), data_index = -1.\n");
            printer->Print("/// Physical columns (data_index >= 0) occupy actual storage.\n");
            printer->Print("struct FieldMeta {\n");
            printer->Indent();
            printer->Print("/// Logical index for uri() mapping, uniquely identifies the field path\n");
            printer->Print("int index{0};\n\n");
            printer->Print("/// Physical column index, -1 for non-storage nodes (messages, maps, repeated containers)\n");
            printer->Print("int data_index{0};\n\n");
            printer->Print("/// Top-level protobuf descriptor (root of the message)\n");
            printer->Print("const google::protobuf::Descriptor* root{nullptr};\n\n");
            printer->Print("/// Full dotted path from root to this field, e.g., \"address.detail.region\"\n");
            printer->Print("std::string path;\n\n");
            printer->Print("/// Corresponding protobuf FieldDescriptor for low-level type info\n");
            printer->Print("const google::protobuf::FieldDescriptor* field{nullptr};\n\n");
            printer->Print("/// Sequence of physical column indices along the path from root to this field\n");
            printer->Print("std::vector<int> column;\n\n");
            printer->Print("/// True if this field is repeated (or map entry repeated)\n");
            printer->Print("bool repeated{false};\n\n");
            printer->Print("/// C++ runtime type string (e.g., \"std::string\", \"int32_t\", \"std::vector<uint8_t>\")\n");
            printer->Print("std::string cpp_type;\n");
            printer->Outdent();
            printer->Print("};\n\n");

            printer->Print(_vars, "static const turbo:flat_hash_map<std::string, FieldMeta>& uri_data();\n\n");

        }

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

    void MessageViewGenerator::generate_struct_transfer(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < nested_generators_.size(); i++) {
            nested_generators_[i]->generate_struct_transfer(printer);
        }

        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// metas \n");
        printer->Print(_vars, "const google::protobuf::Descriptor* $domain$::get_descriptor() {\n");
        printer->Indent();
        printer->Print(_vars, "return $PBTYPE$::descriptor();\n");
        printer->Outdent();
        printer->Print(_vars, "}\n\n");

        if (!_parent) {
            printer->Print(_vars, "/// Returns a static map from field path to FieldMeta.\n");
            printer->Print(_vars, "/// This map is initialized once and provides metadata for all fields.\n");
            printer->Print(_vars, "const turbo::flat_hash_map<std::string, $domain$::FieldMeta>& $domain$::uri_data() {\n");
            printer->Indent();
            printer->Print(_vars, "static const turbo::flat_hash_map<std::string, FieldMeta> kMap = [](){\n");
            printer->Indent();
            printer->Print(_vars, "static const turbo::flat_hash_map<std::string, FieldMeta> map;\n");
            auto &field_metas = GlobalState::instance().field_metas;
            for (auto k = 0; k < field_metas.size(); k++) {
                auto & meta = field_metas.at(k);
                if (meta->path.empty()) {
                    /// root node skip
                    continue;
                }
                printer->Print(_vars, "{\n");
                printer->Indent();
                // Build column vector initializer list
                printer->Print(_vars, turbo::str_format("FieldMeta m%d;\n",k));


                printer->Print(_vars, turbo::str_format("m%d.index = %d;\n",k, meta->index));
                printer->Print(_vars, turbo::str_format("m%d.data_index = %d;\n",k, meta->data_index));
                printer->Print(_vars, turbo::str_format("m%d.path = \"%s\";\n",k, meta->path));
                for (auto it : meta->column) {
                    printer->Print(_vars, turbo::str_format("m%d.column.push_back(%d);\n",k, it));
                }
                printer->Print(_vars, turbo::str_format("m%d.repeated = %s;\n",k, meta->repeated ? "true": "false"));
                printer->Print(_vars, turbo::str_format("m%d.cpp_type = \"%s\";\n",k, meta->cpp_type));
                printer->Print(_vars, turbo::str_format("map[\"%s\"] = m%d;\n",meta->path, k));
                printer->Outdent();
                printer->Print(_vars, "}\n");
            }
            printer->Print(_vars, "return map;\n");
            printer->Outdent();
            printer->Print(_vars, "}();\n\n");
            printer->Print(_vars, "return kMap;\n");
            printer->Outdent();
            printer->Print(_vars, "}\n\n");
        }
    }

    void MessageViewGenerator::
    GenerateMessageDescriptor(google::protobuf::io::Printer *printer) {
    }
} // namespace shark
