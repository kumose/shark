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

#include <shark/rt/bytes_field.h>
#include <shark/rt/enum_field.h>
#include <shark/rt/field_map.h>
#include <shark/utility/helpers.h>
#include <shark/rt/message_field.h>
#include <shark/rt/primitive_field.h>
#include <shark/rt/string_field.h>
#include <shark/utility/compat.h>
#include <shark/rt/map_field.h>
#include <shark/rt/any_field.h>

namespace shark {

    FieldGeneratorMap::FieldGeneratorMap(const google::protobuf::Descriptor *descriptor)
        : descriptor_(descriptor) {
        // Construct all the FieldGenerators.
        for (int i = 0; i < descriptor->field_count(); i++) {
            auto ptr = MakeGenerator(descriptor->field(i));
            ptr->initialize();
            field_generators_.push_back(std::move(ptr));
        }
    }

    std::unique_ptr<FieldGeneratorBase> FieldGeneratorMap::MakeGenerator(const google::protobuf::FieldDescriptor *field) {
        const idl::SharkFieldOptions opt = field->options().GetExtension(idl::shark_field);
        switch (field->type()) {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                    return std::make_unique<MapFieldGenerator>(field);
                }
                if (is_protobuf_any(field)) {
                    return std::make_unique<AnyFieldGenerator>(field);
                }
                return std::make_unique<MessageFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                if (opt.string_as_bytes())
                    return std::make_unique<BytesFieldGenerator>(field);
                else
                    return std::make_unique<StringFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return std::make_unique<BytesFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
                return std::make_unique<EnumFieldGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
                return nullptr; // XXX
            default:
                return std::make_unique<PrimitiveFieldGenerator>(field);
        }
    }

    FieldGeneratorMap::~FieldGeneratorMap() {
    }

    const FieldGeneratorBase &FieldGeneratorMap::get(
        const google::protobuf::FieldDescriptor *field) const {
        KCHECK_EQ(field->containing_type(), descriptor_);
        return *field_generators_[field->index()];
    }
} // namespace shark
