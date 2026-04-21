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

#include <shark/generator/map_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/generator/helpers.h>
#include <shark/generator/string_field.h>

namespace shark {
    MapFieldGenerator::MapFieldGenerator(const google::protobuf::FieldDescriptor *descriptor, std::string message)
        : FieldGenerator(descriptor, message) {
        variables_["name"] = FieldName(descriptor);
        auto mtp = GlobalState::get_ctype(descriptor_);
        variables_["map_type"] = mtp;
    }

    // implements FieldGenerator ---------------------------------------
    void MapFieldGenerator::generate_members(google::protobuf::io::Printer* printer) const {
        printer->Print(variables_, "$deprecated$ $map_type$ _$name$$default_init$;\n");
    }

    void MapFieldGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "$deprecated$ inline const $map_type$& $name$() const;\n");
        printer->Print(variables_, "$deprecated$ inline void set_$name$(const $map_type$ &v);\n");
    }

    void MapFieldGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = std::move(rhs._$name$);\n");
    }
    void MapFieldGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$ = rhs._$name$;\n");
    }



    void MapFieldGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "/// getter\n");
        printer->Print(variables_, "inline const $map_type$& $message$::$name$() const {\n");
        printer->Indent();
        printer->Print(variables_, "return _$name$;\n");
        printer->Outdent();
        printer->Print(variables_, "}\n");
        printer->Print(variables_, "/// setter\n");
        printer->Print(variables_, "inline void $message$::set_$name$(const $map_type$ &v) {\n");
        printer->Indent();
        printer->Print(variables_, "_$name$ = v;\n");
        printer->Outdent();
        printer->Print(variables_, "}\n");
    }

    void MapFieldGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "_$name$.reserve(pb.$name$_size());\n");
        printer->Print(variables_, "for(auto &it : pb.$name$()) {\n");
        printer->Indent();
        printer->Print(variables_, "_$name$[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");

    }

    void MapFieldGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(variables_, "for(auto &it : _$name$) {\n");
        printer->Indent();
        printer->Print(variables_, "(*pb.mutable_$name$())[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");
    }

} // namespace shark
