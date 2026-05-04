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

#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include <shark/idl/shark_options.pb.h>
#include <turbo/strings/str_cat.h>
#include <turbo/strings/str_format.h>
#include <shark/utility/compat.h>

namespace shark {
    // Get the (unqualified) name that should be used for this field in C code.
    // The name is coerced to lower-case to emulate proto1 behavior.  People
    // should be using lowercase-with-underscores style for proto field names
    // anyway, so normally this just returns field->name().
    std::string varify_field_name(const google::protobuf::FieldDescriptor *field);

    std::string field_uri(const google::protobuf::FieldDescriptor *field);

    std::string field_uri_without_namespace(const google::protobuf::FieldDescriptor *field);

    std::string get_message_type(const google::protobuf::Descriptor *descriptor);

    std::string get_enum_type(const google::protobuf::EnumDescriptor *descriptor);

    std::string get_enum_type_parse_func(const google::protobuf::EnumDescriptor *descriptor);

    std::string get_enum_type_domain(const google::protobuf::EnumDescriptor *descriptor);
    std::string get_enum_type_to_string_domain(const google::protobuf::EnumDescriptor *descriptor);

    std::string message_type(const google::protobuf::Descriptor *descriptor, const std::string &suffix);

    std::string relative_message_type(const google::protobuf::Descriptor *d, const google::protobuf::Descriptor *r);

    std::string relative_message_type(const google::protobuf::Descriptor *d, const google::protobuf::Descriptor *r, const std::string &suffix);

    std::string message_domain_without_namespace(const google::protobuf::FieldDescriptor *field);

    std::string message_domain_without_namespace(const google::protobuf::Descriptor *field);

    std::string message_domain_without_namespace(const google::protobuf::FieldDescriptor *field,
                                                 const std::string &suffix);

    std::string message_domain_without_namespace(const google::protobuf::Descriptor *field, const std::string &suffix);


    std::string enum_type(const google::protobuf::FieldDescriptor *field);

    std::string enum_type(const google::protobuf::EnumDescriptor *field);

} // namespace shark
