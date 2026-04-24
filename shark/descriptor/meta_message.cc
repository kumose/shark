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

#pragma once


#include <memory>
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/descriptor/meta_message.h>

namespace shark {
    MetaMessageGenerator::MetaMessageGenerator(const google::protobuf::Descriptor *descriptor,
                                               const std::string &dllexport_decl)
        : MessageGeneratorBase(descriptor, dllexport_decl),
          _field_generators(descriptor) {
        _variables["classname_descriptor"] = _variables["classname"] + "Descriptor";
        _variables["domain_descriptor"] = _variables["classname"] + "Descriptor";
        _variables["domain_extractor"] = _variables["classname"] + "Extractor";
    }

    void MetaMessageGenerator::generate_fwd_typedef(google::protobuf::io::Printer *printer) {
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// Serialize Descriptor \n");
        printer->Print(_variables, "class $classname_descriptor$;\n\n");

        ////////////////////////////////////////////////////////////
        /// extrator
        printer->Print("/// Serialize Extractor \n");
        printer->Print(_variables, "template <int INDEX>\n");
        printer->Print(_variables, "struct $domain_extractor$;\n");
    }

    void MetaMessageGenerator::generate_definition(google::protobuf::io::Printer *printer) {
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// Serialize Descriptor \n");
        printer->Print(_variables, "class $classname_descriptor$ {\n");
        printer->Print("public:\n\n");
        printer->Indent();
        printer->Print(_variables, "static const google::protobuf::Descriptor* get_descriptor();\n\n");

        generate_field_meta_definition(printer);

        ///////////////////////////////////////////
        printer->Print("static const turbo::flat_hash_map<std::string, FieldMeta*>& field_description() {\n\n");
        printer->Indent();
        printer->Print(_variables, "return instance().field_map;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");

        printer->Outdent();
        printer->Print("private:\n");
        printer->Indent();

        generate_private_definition(printer);

        generate_member_definition(printer);

        printer->Outdent();
        printer->Print("};\n");
    }

    void MetaMessageGenerator::generate_private_definition(google::protobuf::io::Printer *printer) {
        printer->Print(_variables, "$classname_descriptor$() {\n");
        printer->Indent();
        printer->Print(_variables, "initialize();\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");
        printer->Print("void initialize();\n\n");
        printer->Print(_variables, "static $classname_descriptor$& instance() {\n");
        printer->Indent();
        printer->Print(_variables, "static $classname_descriptor$ ins;\n");
        printer->Print(_variables, "return ins;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");
    }

    void MetaMessageGenerator::generate_inline_extractor_implement(google::protobuf::io::Printer *printer) {
        printer->Print("\n/////////////////////////////////////////////////////////////\n");
        printer->Print("/// Serialize Extractor \n");
        for (auto &it : _field_generators.metas()) {
            printer->Print(_variables, "template <>\n");
            printer->Print(_variables, turbo::str_format("struct $domain_extractor$<%d> {\n", it->index));
            printer->Indent();
            ////////////////////////////////////////////////////
            /// static constexpr
            std::vector<std::string_view> lvs = turbo::str_split(it->path, ".");
            std::vector<int> lvs2;
            std::vector<FieldMeta*> meta_path;
            auto &meta_map = _field_generators.meta_map();
            for (auto i = 1; i < lvs.size(); i++) {
                auto str = turbo::str_join(lvs.begin(), lvs.begin() + i, ".");
                auto it = meta_map.find(str);
                lvs2.push_back(it->second->index);
                meta_path.push_back(it->second);
            }
            printer->Print(_variables, turbo::str_format("static constexpr bool is_data_column = %s;\n", it->data_index == -1 ? "false" : "true"));
            printer->Print(_variables, turbo::str_format("static constexpr int data_index = %d;\n", it->data_index));
            printer->Print(_variables, turbo::str_format("static constexpr std::string_view path = \"%s\";\n", it->path));
            printer->Print(_variables, turbo::str_format("static constexpr std::array<int, %d> columns = {%s};\n", it->column.size(), turbo::str_join(it->column, ", ")));
            printer->Print(_variables, turbo::str_format("static constexpr bool is_repeat = %s;\n\n", it->repeated ? "true" : "false"));
            printer->Print(_variables, turbo::str_format("static constexpr std::array<int, %d> paths = {%s};\n", lvs2.size(), turbo::str_join(lvs2, ", ")));
            if (it->index == 0 ) {
                printer->Outdent();
                printer->Print(_variables, "};\n\n");
                continue;
            }
            std::string args;
            std::string func;
            for (auto i = 0; i < lvs2.size(); i++) {
                if (meta_path[i]->repeated) {
                    args += turbo::str_format(", int %s_size", lvs[i]);
                    func += turbo::str_format(".%s()[%s_size]",  lvs[i], lvs[i]);
                } else {
                    func += turbo::str_format(".%s()",  lvs[i]);
                }
            }

            if (it->repeated) {
                args += turbo::str_format(", int %s_size", it->field->name());
                func += turbo::str_format(".%s()[%s_size]",  it->field->name(), it->field->name());
            } else {
                func += turbo::str_format(".%s()",  it->field->name());
            }

            printer->Print(_variables, turbo::str_format("///////////////////////////////////////////////////////////\n\n"));
            if (it->data_index == -1) {
                printer->Print(_variables, turbo::str_format("static const %s &extract(const $classname$ &src%s) {\n", get_ctype(it->field, nullptr), args));
            } else {
                printer->Print(_variables, turbo::str_format("static %s extract(const $classname$ &src%s) {\n", get_ctype(it->field, nullptr), args));
            }

            printer->Indent();
            printer->Print(_variables, turbo::str_format("return src%s;\n",func));
            printer->Outdent();
            printer->Print(_variables, "}\n\n");
            printer->Outdent();
            printer->Print(_variables, "};\n\n");

        }

    }

    void MetaMessageGenerator::generate_member_definition(google::protobuf::io::Printer *printer) {
        printer->Print(_variables, "int _data_index{0};\n");
        printer->Print(_variables, "int _index{0};\n");
        printer->Print(_variables, "turbo::flat_hash_map<std::string, FieldMeta*> field_map;\n");
        printer->Print(_variables, "std::vector<std::unique_ptr<FieldMeta> > field_metas;\n");
    }

    void MetaMessageGenerator::generate_field_meta_definition(google::protobuf::io::Printer *printer) {
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
        printer->Print("/// True if this field is map\n");
        printer->Print("bool is_map{false};\n\n");
        printer->Print("/// True if this field is google.protobuf.any\n");
        printer->Print("bool is_any{false};\n\n");
        printer->Print("/// C++ runtime type string (e.g., \"std::string\", \"int32_t\", \"std::vector<uint8_t>\")\n");
        printer->Print("std::string cpp_type;\n");
        printer->Outdent();
        printer->Print("};\n\n");
    }

    void MetaMessageGenerator::generate_inline_implement(google::protobuf::io::Printer *printer) {
        generate_inline_extractor_implement(printer);
    }

    void MetaMessageGenerator::generate_get_descriptor_implement(google::protobuf::io::Printer *printer) {
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// metas \n");
        printer->Print(_variables, "const google::protobuf::Descriptor* $domain_descriptor$::get_descriptor() {\n");
        printer->Indent();
        printer->Print(_variables, "return $PBTYPE$::descriptor();\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");
    }

    void MetaMessageGenerator::generate_initialize_implement(google::protobuf::io::Printer *printer) {
        printer->Print(_variables, "/// Returns a static map from field path to FieldMeta.\n");
        printer->Print(_variables, "/// This map is initialized once and provides metadata for all fields.\n");
        printer->Print(
            _variables, "void $domain_descriptor$::initialize() {\n");
        printer->Indent();
        printer->Print(_variables, "auto descriptor_ = get_descriptor();\n\n");
        printer->Print(_variables, "auto r = std::make_unique<FieldMeta>();\n\n");
        printer->Print(_variables, "auto rptr = r.get();\n");

        printer->Print(_variables, "field_map[\"root\"] = rptr;\n");
        printer->Print(_variables, "auto rptr = r.get();\n");
        printer->Print(_variables, "rptr->index = _index++;\n");
        printer->Print(_variables, "rptr->data_index = _data_index++;\n");
        printer->Print(_variables, "std::deque<FieldMeta*> que;\n");
        printer->Print(_variables, "for (auto i = 0; i < descriptor_->field_count(); ++i ) {\n");
        printer->Indent();
        printer->Print(_variables, "auto field = descriptor_->field(i);\n");
        printer->Print(_variables, "auto meta = std::make_unique<FieldMeta>();\n");
        printer->Print(_variables, "meta->field = field;\n");
        printer->Print(_variables, "meta->repeated = field->is_repeated();\n");
        printer->Print(_variables, "meta->path =field->name();\n");
        printer->Print(_variables, "meta->index = _index++;\n");
        printer->Print(_variables, "meta->is_map = is_protobuf_map(field);\n");
        printer->Print(_variables, "meta->is_any = is_protobuf_any(field);\n");
        printer->Print(_variables, "meta->root = descriptor_;\n");
        printer->Print(_variables, "auto ptr = meta.get();\n");
        printer->Print(_variables, "field_map[meta->path] = ptr;\n");
        printer->Print(_variables, "field_metas.push_back(std::move(meta));\n");
        printer->Print(_variables, "if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {\n");
        printer->Indent();
        printer->Print(_variables, "ptr->data_index = -1;\n");
        printer->Print(_variables, "que.push_back(ptr);\n");
        printer->Print(_variables, "continue;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");
        printer->Print(_variables, "ptr->data_index = _data_index++;\n");
        printer->Print(_variables, "if (field->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {\n");
        printer->Indent();
        printer->Print(_variables, " ptr->cpp_type = shark::cpp_type(field,true);\n");
        printer->Outdent();
        printer->Print(_variables, "} else {\n");
        printer->Indent();
        printer->Print(_variables, " ptr->cpp_type = \"enum\";\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");
        printer->Outdent();
        printer->Print(_variables, "}\n\n");

        printer->Print(_variables, "while (!que.empty()) {\n");
        printer->Indent();

        printer->Print(_variables, "auto entity = que.front();\n");
        printer->Print(_variables, "que.pop_front();\n");
        printer->Print(_variables, "for (auto i = 0; i < entity->field->message_type()->field_count(); ++i) {\n");
        printer->Indent();
        printer->Print(_variables, "auto field = entity->field->message_type()->field(i);\n");
        printer->Print(_variables, "auto meta = std::make_unique<FieldMeta>();\n");
        printer->Print(_variables, "meta->field = field;\n");
        printer->Print(_variables, "meta->path =entity->path + \".\" + field->name();\n");
        printer->Print(_variables, "meta->index = _index++;\n");
        printer->Print(_variables, "meta->is_map = is_protobuf_map(field) || entity->is_map;\n");
        printer->Print(_variables, "meta->is_any = is_protobuf_any(field) || entity->is_map;\n");
        printer->Print(_variables, "meta->root = descriptor_;\n");
        printer->Print(_variables, "entity->column.push_back(meta->index);\n");
        printer->Print(_variables, "auto ptr = meta.get();\n");
        printer->Print(_variables, "field_map[meta->path] = ptr;\n");
        printer->Print(_variables, "field_metas.push_back(std::move(meta));\n");
        printer->Print(_variables, "if (field->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {\n");
        printer->Indent();
        printer->Print(_variables, "ptr->data_index  = -1;\n");
        printer->Print(_variables, "continue;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
        printer->Print(_variables, "ptr->data_index = _data_index++;\n");
        printer->Print(_variables, "if (field->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {\n");
        printer->Indent();
        printer->Print(_variables, "ptr->cpp_type = shark::cpp_type(field,true);\n");
        printer->Outdent();
        printer->Print(_variables, " } else {\n");
        printer->Indent();
        printer->Print(_variables, "ptr->cpp_type = \"enum\";\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
    }

    void MetaMessageGenerator::generate_implement(google::protobuf::io::Printer *printer) {
        generate_get_descriptor_implement(printer);
        generate_initialize_implement(printer);
        //////////////////////////////////
        ///
    }
} // namespace shark
