// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
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

#include <shark/generator/type_mapper.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_lite.h>
namespace shark::generator {
    Type TypeMapper::map_field(const google::protobuf::FieldDescriptor* field) const {
        if (field->is_repeated()) {
            auto elem = map_single_field(field);
            return Type::vector_type(elem);
        }
        if (field->is_map()) {
            const auto* map_desc = field->message_type();
            auto key = map_single_field(map_desc->map_key());
            auto value = map_single_field(map_desc->map_value());
            return Type::unordered_map_type(key, value);
        }
        return map_single_field(field);
    }

    Type TypeMapper::map_single_field(const google::protobuf::FieldDescriptor *field) const {
        switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                return Type::int32_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                return Type::int64_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                return Type::uint32_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                return Type::uint64_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                return Type::float_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                return Type::double_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                return Type::bool_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                return Type::string_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                return map_enum(field->enum_type());
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                return map_message(field->message_type());
            default:
                return Type::void_type();
        }
    }

    Type TypeMapper::map_primitive(google::protobuf::FieldDescriptor::CppType cpp_type) const {
        // Not directly used, kept for potential future use
        switch (cpp_type) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                return Type::int32_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                return Type::int64_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
                return Type::uint32_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
                return Type::uint64_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                return Type::float_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                return Type::double_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                return Type::bool_type();
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                return Type::string_type();
            default:
                return Type::void_type();
        }
    }

    Type TypeMapper::map_message(const google::protobuf::Descriptor *msg) const {
        std::string cpp_name = msg->full_name();
        // Replace '.' with '::' to form C++ namespace separator.
        size_t pos = 0;
        while ((pos = cpp_name.find('.', pos)) != std::string::npos) {
            cpp_name.replace(pos, 1, "::");
            pos += 2;
        }
        return Type::user_defined(cpp_name);
    }

    Type TypeMapper::map_enum(const google::protobuf::EnumDescriptor *enum_desc) const {
        // Use fully qualified enum name.
        return Type::enum_type(enum_desc->full_name());
    }
} // namespace shark::generator
