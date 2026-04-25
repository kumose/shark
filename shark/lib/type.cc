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

#include <shark/type.h>
#include <shark/uri.h>
#include <turbo/strings/ascii.h>
#include <turbo/strings/str_split.h>
#include <turbo/strings/str_replace.h>

namespace shark {
    bool is_protobuf_map(const google::protobuf::FieldDescriptor *field) {
        if (field->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE || !field->is_repeated()) {
            return false;
        }
        const google::protobuf::Descriptor *entry_desc = field->message_type();
        if (entry_desc == nullptr) {
            return false;
        }
        if (entry_desc->field_count() != 2) {
            return false;
        }
        const google::protobuf::FieldDescriptor *key_desc = entry_desc->field(KEY_INDEX);
        if (nullptr == key_desc
            || key_desc->is_repeated()
            || key_desc->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_STRING
            || KEY_NAME != key_desc->name()) {
            return false;
        }
        const google::protobuf::FieldDescriptor *value_desc = entry_desc->field(VALUE_INDEX);
        if (nullptr == value_desc
            || VALUE_NAME != value_desc->name()) {
            return false;
        }
        return true;
    }

    const google::protobuf::FieldDescriptor *protobuf_map_key(const google::protobuf::FieldDescriptor *field) {
        const google::protobuf::Descriptor *entry_desc = field->message_type();

        const google::protobuf::FieldDescriptor *key_desc = entry_desc->field(KEY_INDEX);
        return key_desc;
    }

    const google::protobuf::FieldDescriptor *protobuf_map_value(const google::protobuf::FieldDescriptor *field) {
        const google::protobuf::Descriptor *entry_desc = field->message_type();
        const google::protobuf::FieldDescriptor *value_desc = entry_desc->field(VALUE_INDEX);
        return value_desc;
    }

    bool is_protobuf_any(const google::protobuf::FieldDescriptor *field) {
        if (field->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
            return false;
        }
        const google::protobuf::Descriptor *entry_desc = field->message_type();
        if (entry_desc == nullptr) {
            return false;
        }
        if (entry_desc->field_count() != 2) {
            return false;
        }
        const google::protobuf::FieldDescriptor *key_desc = entry_desc->field(KEY_INDEX);
        if (nullptr == key_desc
            || key_desc->is_repeated()
            || key_desc->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_STRING
            || TYPE_URL != key_desc->name()) {
            return false;
        }

        const google::protobuf::FieldDescriptor *value_desc = entry_desc->field(VALUE_INDEX);
        if (nullptr == value_desc
            || VALUE_NAME != value_desc->name()) {
            return false;
        }

        return true;
    }

    bool is_primitive_type(google::protobuf::FieldDescriptor::Type type) {
        switch (type) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
                return true;
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_STRING:
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                return false;

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    bool is_cpp_type(google::protobuf::FieldDescriptor::Type type) {
        if (is_primitive_type(type)) {
            return true;
        }
        switch (type) {
            case google::protobuf::FieldDescriptor::TYPE_STRING:
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return true;
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                return false;

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    std::string get_ctype(const google::protobuf::FieldDescriptor *descriptor,const google::protobuf::Descriptor *relative) {
        if (is_cpp_type(descriptor->type())) {
            return cpp_type(descriptor);
        }
        if (descriptor->type() == google::protobuf::FieldDescriptor::TYPE_ENUM) {
            return enum_type(descriptor, relative, true);
        }
        ////////////////
        /// google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        if (is_protobuf_map(descriptor)) {
            return map_type(descriptor, relative,true);
        }
        if (is_protobuf_any(descriptor)) {
            return "std::pair<std::string, std::string>";
        }
        return relative_message_type(descriptor->message_type(), relative);
    }

    std::string cpp_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try) {
        auto opt = descriptor->options();
        auto ext_opt = opt.GetExtension(idl::shark_field);
        auto opt_type = ext_opt.ctype();
        if (!opt_type.empty()) {
            return opt_type;
        }
        switch (descriptor->type()) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
                return "int32_t";
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
                return "int64_t";
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
                return "uint32_t";
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
                return "uint64_t";
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                return "float";
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                return "double";
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
                return "bool";
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                return "std::string";
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return "std::vector<uint8_t>";
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                if (!is_try) {
                    KLOG(FATAL) << "not a primitive type:" << static_cast<int>(descriptor->type());
                }
                return "";

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    std::string enum_type(const google::protobuf::FieldDescriptor *descriptor, const google::protobuf::Descriptor *relative ,bool is_try) {
        if (descriptor->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {
            if (is_try) {
                return "";
            }
            KLOG(FATAL) << "must spefic enum or message type";
        }
        std::string ff = descriptor->enum_type()->full_name();
        auto ef = turbo::str_replace_all(ff, {{".", "::"}});

        std::string rm;
        if (relative) {
            rm = message_type(relative) + "::";
        }
        return std::string(turbo::strip_prefix(ef, rm));
    }


    std::string map_type(const google::protobuf::FieldDescriptor *descriptor, const google::protobuf::Descriptor *relative , bool is_try) {
        // Get the map entry message
        const auto *entry = descriptor->message_type();
        const auto *key_field = entry->field(0);
        const auto *value_field = entry->field(1);

        // Recursively get C++ types for key and value
        std::string key_type = get_ctype(key_field, relative);
        std::string value_type = get_ctype(value_field, relative);
        if (key_type.empty() || value_type.empty()) {
            if (!is_try)
                KLOG(FATAL) << "failed to get key/value type for map: " << descriptor->full_name();
            return "";
        }

        // Obtain sk_map_type template from the file that defines this map
        const auto *file = descriptor->file();
        auto file_opts = file->options().GetExtension(idl::shark_file);
        std::string map_template = "std::unordered_map";
        if (file_opts.has_sk_map_type()) {
            map_template = file_opts.sk_map_type();
        }

        // Replace placeholders
        size_t pos = map_template.find("<");
        std::string mtype = map_template;
        if (pos != std::string::npos) {
            mtype = map_template.substr(0, pos);
        }

        return turbo::str_cat(mtype, "<", key_type, ",", value_type, ">");
    }
} // namespace shark
