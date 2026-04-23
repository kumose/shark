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
    }

    void MetaMessageGenerator::generate_fwd_typedef(google::protobuf::io::Printer *printer) {
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// serialize meta \n");
        printer->Print("class $classname$Meta;\n");
    }

    void MetaMessageGenerator::generate_definition(google::protobuf::io::Printer *printer) {
        printer->Print("///////////////////////////////////////////////////////////////////////// \n");
        printer->Print("/// serialize meta \n");
        printer->Print(_variables, "class $classname$Meta {\n");
        printer->Print("public:\n");
        printer->Print("};\n");
    }
} // namespace shark

/*
 *if (!_parent) {
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
        }*/


/*
 * if (!_parent) {
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

        }*/
