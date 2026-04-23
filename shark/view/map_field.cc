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

#include <shark/view/map_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <shark/view/string_field.h>

namespace shark {
    MapFieldViewGenerator::MapFieldViewGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldMetaGenerator(descriptor) {
        auto mtp = get_ctype(descriptor_, descriptor_->containing_type());
        auto k = protobuf_map_key(descriptor_);
        auto v = protobuf_map_value(descriptor_);
        variables_["map_type"] = mtp;
        variables_["key_type"] = get_ctype(k, descriptor_->containing_type());
        variables_["value_type"] = get_ctype(v, descriptor_->containing_type());
    }

    // implements FieldGenerator ---------------------------------------
    void MapFieldViewGenerator::generate_members(google::protobuf::io::Printer* printer) const {
        printer->Print(variables_, "$deprecated$shark::MapView<$key_type$,$value_type$> _$name$$default_init$;\n");
    }

    void MapFieldViewGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "$deprecated$inline shark::MapView<$key_type$,$value_type$> $name$() const;\n");
    }

    void MapFieldViewGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
    }
    void MapFieldViewGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }



    void MapFieldViewGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "/// getter\n");
        printer->Print(variables_, "inline shark::MapView<$key_type$,$value_type$> $domain_view$::$name$() const {\n");
        printer->Indent();
        printer->Print(variables_, "return _$name$;\n");
        printer->Outdent();
        printer->Print(variables_, "}\n");
    }

    void MapFieldViewGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$.reserve(pb.$name$_size());\n");
        printer->Print(variables_, "for(auto &it : pb.$name$()) {\n");
        printer->Indent();
        printer->Print(variables_, "_$name$[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");

    }

    void MapFieldViewGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "for(auto &it : _$name$) {\n");
        printer->Indent();
        printer->Print(variables_, "(*pb.mutable_$name$())[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");
    }

} // namespace shark
