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

#pragma once

#include "shark/generator/type.h"

#include <google/protobuf/descriptor.h>

namespace shark::generator {

/// Converts protobuf field descriptors to Type objects.
class TypeMapper {
public:
    /// Maps a field descriptor to a Type.
    /// Handles repeated, map, primitive, message, and enum fields.
    Type map_field(const google::protobuf::FieldDescriptor* field) const;

private:
    Type map_primitive(google::protobuf::FieldDescriptor::CppType cpp_type) const;
    Type map_message(const google::protobuf::Descriptor* msg) const;
    Type map_enum(const google::protobuf::EnumDescriptor* enum_desc) const;
    Type map_single_field(const google::protobuf::FieldDescriptor* field) const;
};

}  // namespace shark::generator