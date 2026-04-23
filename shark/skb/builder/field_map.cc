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

#include <shark/skb/builder/bytes_field.h>
#include <shark/skb/builder/enum_field.h>
#include <shark/skb/builder/field_map.h>
#include <shark/utility/helpers.h>
#include <shark/skb/builder/message_field.h>
#include <shark/skb/builder/primitive_field.h>
#include <shark/skb/builder/string_field.h>
#include <shark/utility/compat.h>
#include <shark/skb/builder/map_field.h>
#include <shark/skb/builder/any_field.h>

namespace shark {

    FieldSkbGeneratorMap::FieldSkbGeneratorMap(const google::protobuf::Descriptor *descriptor)
        : descriptor_(descriptor) {
        // Construct all the FieldGenerators.
        for (int i = 0; i < descriptor->field_count(); i++) {
            auto ptr = MakeGenerator(descriptor->field(i));
            ptr->initialize();
            field_generators_.push_back(std::move(ptr));
        }
    }

    std::unique_ptr<FieldGeneratorBase> FieldSkbGeneratorMap::MakeGenerator(const google::protobuf::FieldDescriptor *field) {
        const idl::SharkFieldOptions opt = field->options().GetExtension(idl::shark_field);
        switch (field->type()) {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                if (field->is_map()) {
                    return std::make_unique<MapFieldSkbGenerator>(field);
                }
                if (is_protobuf_any(field)) {
                    return std::make_unique<AnyFieldSkbGenerator>(field);
                }
                return std::make_unique<MessageFieldSkbGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                if (opt.string_as_bytes())
                    return std::make_unique<BytesFieldSkbGenerator>(field);
                else
                    return std::make_unique<StringFieldSkbGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return std::make_unique<BytesFieldSkbGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
                return std::make_unique<EnumFieldSkbGenerator>(field);
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
                return nullptr; // XXX
            default:
                return std::make_unique<PrimitiveFieldSkbGenerator>(field);
        }
    }

    FieldSkbGeneratorMap::~FieldSkbGeneratorMap() {
    }

    const FieldGeneratorBase &FieldSkbGeneratorMap::get(
        const google::protobuf::FieldDescriptor *field) const {
        KCHECK_EQ(field->containing_type(), descriptor_);
        return *field_generators_[field->index()];
    }
} // namespace shark
