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

    static constexpr std::string_view KEY_NAME = "key";
    static constexpr std::string_view VALUE_NAME = "value";
    static constexpr int KEY_INDEX = 0;
    static constexpr int VALUE_INDEX = 1;

    static constexpr std::string_view TYPE_URL = "type_url";
    static constexpr std::string_view A_TYPE_URL = "@type";

    bool is_protobuf_map(const google::protobuf::FieldDescriptor *field);

    bool is_protobuf_any(const google::protobuf::FieldDescriptor *field);

    bool is_primitive_type(google::protobuf::FieldDescriptor::Type type);

    bool is_cpp_type(google::protobuf::FieldDescriptor::Type type);

    const google::protobuf::FieldDescriptor* protobuf_map_key(const google::protobuf::FieldDescriptor *field);

    const google::protobuf::FieldDescriptor* protobuf_map_value(const google::protobuf::FieldDescriptor *field);

    std::string get_ctype(const google::protobuf::FieldDescriptor *descriptor,const google::protobuf::Descriptor *relative);

    std::string cpp_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try = false);

    std::string enum_type(const google::protobuf::FieldDescriptor *descriptor, const google::protobuf::Descriptor *relative ,bool is_try = false);

    std::string map_type(const google::protobuf::FieldDescriptor *descriptor, const google::protobuf::Descriptor *relative , bool is_try = false);

} // namespace shark
