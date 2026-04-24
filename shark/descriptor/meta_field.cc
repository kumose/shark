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

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <shark/descriptor/meta_field.h>

namespace shark {
    // ===================================================================

    MetaFieldDescriptorGenerator::
    MetaFieldDescriptorGenerator(const google::protobuf::FieldDescriptor *descriptor, const FieldMetaMap *map,
                                 std::string index)
        : FieldDescriptorGenerator(descriptor, map, index) {
        auto &meta_map = _field_meta_map->meta_map();
        for (auto i = 1; i < lvs.size(); i++) {
            auto str = turbo::str_join(lvs.begin(), lvs.begin() + i, ".");
            auto it = meta_map.find(str);
            lvs2.push_back(it->second->index);
            meta_path.push_back(it->second);
        }
        lvs = turbo::str_split(_path, ".");
        _meta = meta_map.find(_path)->second;
    }

    MetaFieldDescriptorGenerator::~MetaFieldDescriptorGenerator() {
    }

    void MetaFieldDescriptorGenerator::generate_meta_definition_inline(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "template <>\n");
        printer->Print(_variables, turbo::str_format("struct $domain_extractor$<%d> {\n", _meta->index));
        printer->Indent();
        generate_meta_static_defines(printer);
        generate_meta_function_defines(printer);
        printer->Outdent();
        printer->Print(_variables, "};\n\n");
    }

    void MetaFieldDescriptorGenerator::generate_meta_static_defines(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, turbo::str_format("static constexpr bool is_data_column = %s;\n",
                                                     _meta->data_index == -1 ? "false" : "true"));
        printer->Print(_variables, turbo::str_format("static constexpr int data_index = %d;\n", _meta->data_index));
        printer->Print(
            _variables, turbo::str_format("static constexpr std::string_view path = \"%s\";\n", _meta->path));
        printer->Print(_variables, turbo::str_format("static constexpr std::array<int, %d> columns = {%s};\n",
                                                     _meta->column.size(), turbo::str_join(_meta->column, ", ")));
        printer->Print(_variables, turbo::str_format("static constexpr bool is_repeat = %s;\n\n",
                                                     _meta->repeated ? "true" : "false"));
        printer->Print(_variables, turbo::str_format("static constexpr std::array<int, %d> paths = {%s};\n",
                                                     lvs2.size(), turbo::str_join(lvs2, ", ")));
    }

    void MetaFieldDescriptorGenerator::generate_meta_function_defines(google::protobuf::io::Printer *printer) const {
        if (_meta->index == 0 || _meta->data_index == -1) {
            return;
        }


        printer->Print(_variables, turbo::str_format("///////////////////////////////////////////////////////////\n\n"));
        printer->Print(_variables, turbo::str_format(
                           "static void extract(std::vector<int64_t> &sharp, std::vector<%s> *result) {\n",
                           get_ctype(_meta->field, nullptr)));
        printer->Indent();
        printer->Print(_variables, turbo::str_format("shark::LayoutTopology sharp_builder;\n"));
        int i = 0;
        if (meta_path[i]->repeated) {
            printer->Print(_variables, turbo::str_format("sharp_builder.add_node({},  %s_size());\n", lvs[i]));
            printer->Print(_variables, turbo::str_format("auto &%s_r = src.%s();\n", lvs[i], lvs[i])) ;
            printer->Print(_variables, turbo::str_format("for(auto i = 0; i < %s_r.size(); i++) {\n", lvs[i]));
            printer->Indent();
            printer->Print(_variables, turbo::str_format("for(auto i = 0; i < %s_r.size(); i++) {\n", lvs[i]));
            printer->Outdent();
            printer->Print(_variables, turbo::str_format("}\n"));
        } else {
            func += turbo::str_format(".%s()",  lvs[i]);
        }
        printer->Outdent();
    }
} // namespace shark
