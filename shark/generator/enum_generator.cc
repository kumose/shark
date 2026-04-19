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

#include "shark/generator/enum_generator.h"

#include <shark/ast/cpp_enum.h>
#include <shark/ast/cpp_expression.h>
#include <shark/ast/cpp_type.h>
#include <shark/ast/cpp_type.h>

namespace shark::generator {
    EnumGenerator::EnumGenerator(const google::protobuf::EnumDescriptor *enum_desc)
        : enum_desc_(enum_desc) {
    }

    std::unique_ptr<cppast::CppEntity> EnumGenerator::generate(cppast::CppEntityIndex &index) {
        // Use uint32_t as underlying type (default)
        auto underlying_type = cppast::CppBuiltinType::build(cppast::cpp_uint);
        bool scoped = true; // enum class

        cppast::CppEnum::builder builder(enum_desc_->name(), scoped, std::move(underlying_type),
                                         /*explicit_type=*/true);

        for (int i = 0; i < enum_desc_->value_count(); ++i) {
            const auto *val = enum_desc_->value(i);
            auto value_expr = cppast::CppLiteralExpression::build(
                cppast::CppBuiltinType::build(cppast::cpp_int),
                std::to_string(val->number()));
            auto enum_value = cppast::CppEnumValue::build(index,
                                                          cppast::CppEntityId(val->name()),
                                                          val->name(),
                                                          std::move(value_expr));
            builder.add_value(std::move(enum_value));
        }

        // Finish the enum (it is a definition). We provide a dummy semantic_parent,
        // the actual parent will be set when the node is added to a container.
        auto enum_node = builder.finish(index, cppast::CppEntityId(enum_desc_->name()),
                                        /*semantic_parent=*/{});
        return enum_node;
    }
} // namespace shark::generator
