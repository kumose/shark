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



#include <map>

#include <google/protobuf/io/printer.h>

#include <shark/rt/enum.h>
#include <shark/utility/helpers.h>

namespace shark {
    EnumGenerator::EnumGenerator(const google::protobuf::EnumDescriptor *descriptor,
                                 const std::string &dllexport_decl)
        : descriptor_(descriptor),
          dllexport_decl_(dllexport_decl) {
    }

    EnumGenerator::~EnumGenerator() {
    }

    void EnumGenerator::generate_definition(google::protobuf::io::Printer *printer) {
        std::map<std::string, std::string> vars;
        vars["shortname"] = std::string(descriptor_->name());

        auto opt = descriptor_->options().GetExtension(idl::shark_enum);
        std::string ut = "uint32_t";
        switch (opt.underlying_type()) {
            case idl::EnumUnderlyingType::ENUM_UNDERLYING_U8:
                ut = "uint8_t";
                break;
            case idl::EnumUnderlyingType::ENUM_UNDERLYING_U16:
                ut = "uint16_t";
                break;
            case idl::EnumUnderlyingType::ENUM_UNDERLYING_U32:
            case idl::EnumUnderlyingType::ENUM_UNDERLYING_UNSPECIFIED:
            default:
                /// ut = "uint32_t";
                break;
        }
        vars["UT"] = ut;
        google::protobuf::SourceLocation sourceLoc;
        descriptor_->GetSourceLocation(&sourceLoc);
        PrintComment(printer, sourceLoc.leading_comments);

        printer->Print(vars, "enum class $shortname$ : $UT${\n");
        printer->Indent();

        const google::protobuf::EnumValueDescriptor *min_value = descriptor_->value(0);
        const google::protobuf::EnumValueDescriptor *max_value = descriptor_->value(0);


        vars["opt_comma"] = ",";
        for (int i = 0; i < descriptor_->value_count(); i++) {
            vars["name"] = std::string(descriptor_->value(i)->name());
            vars["number"] = turbo::str_cat(descriptor_->value(i)->number());
            if (i + 1 == descriptor_->value_count())
                vars["opt_comma"] = "";

            google::protobuf::SourceLocation valSourceLoc;
            descriptor_->value(i)->GetSourceLocation(&valSourceLoc);

            PrintComment(printer, valSourceLoc.leading_comments);
            PrintComment(printer, valSourceLoc.trailing_comments);
            printer->Print(vars, "$name$ = $number$$opt_comma$\n");

            if (descriptor_->value(i)->number() < min_value->number()) {
                min_value = descriptor_->value(i);
            }
            if (descriptor_->value(i)->number() > max_value->number()) {
                max_value = descriptor_->value(i);
            }
        }

        printer->Print(vars, "");
        printer->Outdent();
        printer->Print(vars, "};  // $classname$\n");
    }

    void EnumGenerator::generate_inline_declarations(google::protobuf::io::Printer* printer,
                                                 const google::protobuf::Descriptor* parent) {
        if (parent == nullptr) return;  // Global enum, no declaration needed inside a class.
        std::map<std::string, std::string> vars;
        vars["fullname"] =descriptor_->name();
        printer->Print(vars, "static std::string_view to_string($fullname$ value);\n");
    }

    void EnumGenerator::generate_inline_definition(google::protobuf::io::Printer* printer,
                                               const google::protobuf::Descriptor* parent) {
        std::map<std::string, std::string> vars;
        vars["shortname"] = descriptor_->name();
        std::string prefix;
        if (parent != nullptr) {
            prefix = parent->name() + "::";
        }
        vars["PREFIX"] = prefix;

        printer->Print(vars, "inline std::string_view $PREFIX$to_string($PREFIX$$shortname$ value) {\n");
        printer->Indent();
        printer->Print("switch (value) {\n");
        for (int i = 0; i < descriptor_->value_count(); ++i) {
            vars["enumerator"] = descriptor_->value(i)->name();
            printer->Print(vars, "  case $PREFIX$$shortname$::$enumerator$: return \"$enumerator$\";\n");
        }
        printer->Print("  default: return \"UNKNOWN\";\n");
        printer->Print("}\n");
        printer->Outdent();
        printer->Print("}\n");
    }
    struct ValueIndex {
        int value;
        unsigned index;
        unsigned final_index; /* index in uniqified array of values */
        compat::StringView name;
    };

    void EnumGenerator::GenerateValueInitializer(google::protobuf::io::Printer *printer, int index) {

    }

    void EnumGenerator::GenerateEnumDescriptor(google::protobuf::io::Printer *printer) {

    }
} // namespace shark
