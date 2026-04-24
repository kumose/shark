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

#include <shark/builder/map_field.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/escaping.h>
#include <shark/utility/helpers.h>
#include <shark/builder/string_field.h>

namespace shark {
    MapFieldSkbGenerator::MapFieldSkbGenerator(const google::protobuf::FieldDescriptor *descriptor)
        : FieldNoMetaGenerator(descriptor) {
        auto mtp = get_ctype(descriptor_, descriptor_->containing_type());
        _variables["map_type"] = mtp;
    }

    // implements FieldGenerator ---------------------------------------
    void MapFieldSkbGenerator::generate_members(google::protobuf::io::Printer* printer) const {
        printer->Print(_variables, "$deprecated$$map_type$ _$name$$default_init$;\n");
    }

    void MapFieldSkbGenerator::generate_members_declares(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "$deprecated$inline const $map_type$& $name$() const;\n");
        printer->Print(_variables, "$deprecated$inline void set_$name$(const $map_type$ &v);\n");
    }

    void MapFieldSkbGenerator::generate_move_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = std::move(rhs._$name$);\n");
    }
    void MapFieldSkbGenerator::generate_copy_ctor_define(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$ = rhs._$name$;\n");
    }



    void MapFieldSkbGenerator::generate_members_inline_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "/// getter\n");
        printer->Print(_variables, "inline const $map_type$& $domain_skb$::$name$() const {\n");
        printer->Indent();
        printer->Print(_variables, "return _$name$;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
        printer->Print(_variables, "/// setter\n");
        printer->Print(_variables, "inline void $domain_skb$::set_$name$(const $map_type$ &v) {\n");
        printer->Indent();
        printer->Print(_variables, "_$name$ = v;\n");
        printer->Outdent();
        printer->Print(_variables, "}\n");
    }

    void MapFieldSkbGenerator::generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "_$name$.reserve(pb.$name$_size());\n");
        printer->Print(_variables, "for(auto &it : pb.$name$()) {\n");
        printer->Indent();
        printer->Print(_variables, "_$name$[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");

    }

    void MapFieldSkbGenerator::generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const {
        printer->Print(_variables, "for(auto &it : _$name$) {\n");
        printer->Indent();
        printer->Print(_variables, "(*pb.mutable_$name$())[it.first] = it.second;\n");
        printer->Outdent();
        printer->Print("}\n");
    }

} // namespace shark
