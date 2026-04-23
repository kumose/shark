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


#include <shark/generator/field_meta.h>

namespace shark {
    void FieldMetaGenerator::generate_meta_declares(google::protobuf::io::Printer *printer) const {
        /*
        printer->Print(variables_, "\n//////////////////////////////////////////////////////\n");
        printer->Print(variables_, "/// $name$ region\n");
        printer->Print(variables_, "/// Returns the fully-qualified field name (URI).\n");
        printer->Print(variables_, "std::string_view $name$_uri() const;\n\n");
        printer->Print(variables_, "/// Returns the field number (index) as defined in .proto.\n");
        printer->Print(variables_, "int $name$_index() const;\n\n");
        printer->Print(variables_, "/// Returns the field number (index) as defined in .proto.\n");
        printer->Print(variables_, "std::string_view $name$_path() const;\n\n");
        */
    }

    void FieldMetaGenerator::generate_meta_definition(google::protobuf::io::Printer *printer) const {

    }

    void FieldMetaGenerator::generate_meta_definition_inline(google::protobuf::io::Printer *printer) const {
        /*
        printer->Print(variables_, "\n//////////////////////////////////////////////////////\n");
        printer->Print(variables_, "/// $name$ region\n");
        printer->Print(variables_, "inline std::string_view $domain_view$::$name$_uri() const {\n");
        printer->Indent();
        printer->Print(variables_, turbo::str_format("return \"%s\";\n", descriptor_->full_name().c_str()));
        printer->Outdent();
        printer->Print(variables_, "}\n\n");

        printer->Print(variables_, "inline int $domain_view$::$name$_index() const {\n");
        printer->Indent();
        printer->Print(variables_, turbo::str_format("return %d;\n", GlobalState::instance().field_meta_map[descriptor_].index));
        printer->Outdent();
        printer->Print(variables_, "}\n\n");

        printer->Print(variables_, "inline std::string_view $domain_view$::$name$_path() const {\n");
        printer->Indent();
        printer->Print(variables_, turbo::str_format("return \"%s\";\n", GlobalState::instance().field_meta_map[descriptor_].path));
        printer->Outdent();
        printer->Print(variables_, "}\n\n");
        */
    }
}  // namespace shark
