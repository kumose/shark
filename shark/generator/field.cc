// SPDX-License-Identifier: BSD-3-Clause
// Based on Google Protobuf (https://github.com/protocolbuffers/protobuf) and protobuf-c
// (https://github.com/protobuf-c/protobuf-c)
// Copyright 2008 Google Inc., 2008-2025 protobuf-c authors. Modifications for C++ generation.
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
#include <google/protobuf/stubs/common.h>

#include <shark/idl/shark_options.pb.h>

#include <shark/generator/bytes_field.h>
#include <shark/generator/enum_field.h>
#include <shark/generator/field.h>
#include <shark/generator/helpers.h>
#include <shark/generator/message_field.h>
#include <shark/generator/primitive_field.h>
#include <shark/generator/string_field.h>
#include <shark/generator/compat.h>
#include <shark/generator/map_field.h>
#include <shark/generator/any_field.h>

namespace shark {
    FieldGenerator::~FieldGenerator() {
    }

    static bool is_packable_type(google::protobuf::FieldDescriptor::Type type) {
        return type == google::protobuf::FieldDescriptor::TYPE_DOUBLE
               || type == google::protobuf::FieldDescriptor::TYPE_FLOAT
               || type == google::protobuf::FieldDescriptor::TYPE_INT64
               || type == google::protobuf::FieldDescriptor::TYPE_UINT64
               || type == google::protobuf::FieldDescriptor::TYPE_INT32
               || type == google::protobuf::FieldDescriptor::TYPE_FIXED64
               || type == google::protobuf::FieldDescriptor::TYPE_FIXED32
               || type == google::protobuf::FieldDescriptor::TYPE_BOOL
               || type == google::protobuf::FieldDescriptor::TYPE_UINT32
               || type == google::protobuf::FieldDescriptor::TYPE_ENUM
               || type == google::protobuf::FieldDescriptor::TYPE_SFIXED32
               || type == google::protobuf::FieldDescriptor::TYPE_SFIXED64
               || type == google::protobuf::FieldDescriptor::TYPE_SINT32
               || type == google::protobuf::FieldDescriptor::TYPE_SINT64;
        //TYPE_BYTES
        //TYPE_STRING
        //TYPE_GROUP
        //TYPE_MESSAGE
    }

    void FieldGenerator::GenerateDescriptorInitializerGeneric(google::protobuf::io::Printer *printer,
                                                              bool optional_uses_has,
                                                              const std::string &type_macro,
                                                              const std::string &descriptor_addr) const {
        std::map<std::string, std::string> variables;
        const google::protobuf::OneofDescriptor *oneof = descriptor_->containing_oneof();
        const idl::SharkFileOptions opt = descriptor_->file()->options().GetExtension(idl::shark_file);
        variables["TYPE"] = type_macro;
        variables["classname"] = FullNameToC(FieldScope(descriptor_)->full_name(), FieldScope(descriptor_)->file());
        variables["name"] = FieldName(descriptor_);
        if (opt.use_oneof_field_name())
            variables["proto_name"] = std::string(oneof->name());
        else
            variables["proto_name"] = std::string(descriptor_->name());
        variables["descriptor_addr"] = descriptor_addr;
        variables["value"] = SimpleItoa(descriptor_->number());
        if (oneof != NULL)
            variables["oneofname"] = CamelToLower(oneof->name());

        if (FieldSyntax(descriptor_) == 3 &&
            descriptor_->label() == google::protobuf::FieldDescriptor::LABEL_OPTIONAL) {
            variables["LABEL"] = "NONE";
            optional_uses_has = false;
        } else {
            variables["LABEL"] = CamelToUpper(GetLabelName(descriptor_->label()));
        }

        variables["flags"] = "0";

        if (descriptor_->label() == google::protobuf::FieldDescriptor::LABEL_REPEATED
            && is_packable_type(descriptor_->type())
            && descriptor_->options().packed()) {
            variables["flags"] += " | PROTOBUF_C_FIELD_FLAG_PACKED";
        } else if (descriptor_->label() == google::protobuf::FieldDescriptor::LABEL_REPEATED
                   && is_packable_type(descriptor_->type())
                   && FieldSyntax(descriptor_) == 3
                   && !descriptor_->options().has_packed()) {
            variables["flags"] += " | PROTOBUF_C_FIELD_FLAG_PACKED";
        }

        if (descriptor_->options().deprecated())
            variables["flags"] += " | PROTOBUF_C_FIELD_FLAG_DEPRECATED";

        if (oneof != NULL)
            variables["flags"] += " | PROTOBUF_C_FIELD_FLAG_ONEOF";

        // Eliminate codesmell "or with 0"
        if (variables["flags"].find("0 | ") == 0) {
            variables["flags"].erase(0, 4);
        }

        printer->Print("{\n");
        if (descriptor_->file()->options().has_optimize_for() &&
            descriptor_->file()->options().optimize_for() ==
            google::protobuf::FileOptions_OptimizeMode_CODE_SIZE) {
            printer->Print("  NULL, /* CODE_SIZE */\n");
        } else {
            printer->Print(variables, "  \"$proto_name$\",\n");
        }
        printer->Print(variables,
                       "  $value$,\n"
                       "  PROTOBUF_C_LABEL_$LABEL$,\n"
                       "  PROTOBUF_C_TYPE_$TYPE$,\n");
        switch (descriptor_->label()) {
            case google::protobuf::FieldDescriptor::LABEL_REQUIRED:
                printer->Print(variables, "  0,   /* quantifier_offset */\n");
                break;
            case google::protobuf::FieldDescriptor::LABEL_OPTIONAL:
                if (oneof != NULL) {
                    printer->Print(variables, "  offsetof($classname$, $oneofname$_case),\n");
                } else if (optional_uses_has) {
                    printer->Print(variables, "  offsetof($classname$, has_$name$),\n");
                } else {
                    printer->Print(variables, "  0,   /* quantifier_offset */\n");
                }
                break;
            case google::protobuf::FieldDescriptor::LABEL_REPEATED:
                printer->Print(variables, "  offsetof($classname$, n_$name$),\n");
                break;
        }
        printer->Print(variables, "  offsetof($classname$, $name$),\n");
        printer->Print(variables, "  $descriptor_addr$,\n");
        printer->Print(variables, "  $default_value$,\n");
        printer->Print(variables, "  $flags$,             /* flags */\n");
        printer->Print(variables, "  0,NULL,NULL    /* reserved1,reserved2, etc */\n");
        printer->Print("},\n");
    }

    FieldGeneratorMap::FieldGeneratorMap(const google::protobuf::Descriptor *descriptor, std::string message)
        : descriptor_(descriptor) {
        // Construct all the FieldGenerators.
        for (int i = 0; i < descriptor->field_count(); i++) {
            auto ptr = MakeGenerator(descriptor->field(i), message);
            field_generators_.push_back(std::move(ptr));
        }
    }

    std::unique_ptr<FieldGenerator> FieldGeneratorMap::MakeGenerator(const google::protobuf::FieldDescriptor *field,
                                                                     std::string message) {
        const idl::SharkFieldOptions opt = field->options().GetExtension(idl::shark_field);
        switch (field->type()) {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                    return std::make_unique<MapFieldGenerator>(field, message);
                }
                if (is_protobuf_any(field)) {
                    return std::make_unique<AnyFieldGenerator>(field, message);
                }
                return std::make_unique<MessageFieldGenerator>(field, message);
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                if (opt.string_as_bytes())
                    return std::make_unique<BytesFieldGenerator>(field, message);
                else
                    return std::make_unique<StringFieldGenerator>(field, message);
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return std::make_unique<BytesFieldGenerator>(field, message);
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
                return std::make_unique<EnumFieldGenerator>(field, message);
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
                return nullptr; // XXX
            default:
                return std::make_unique<PrimitiveFieldGenerator>(field, message);
        }
    }

    FieldGeneratorMap::~FieldGeneratorMap() {
    }

    const FieldGenerator &FieldGeneratorMap::get(
        const google::protobuf::FieldDescriptor *field) const {
        GOOGLE_CHECK_EQ(field->containing_type(), descriptor_);
        return *field_generators_[field->index()];
    }
} // namespace shark
