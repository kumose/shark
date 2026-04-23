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


#include <map>
#include <string>

#include <shark/generator/field_map.h>

namespace shark {
    class PrimitiveFieldSkbGenerator : public FieldMetaGenerator {
    public:
        explicit PrimitiveFieldSkbGenerator(const google::protobuf::FieldDescriptor *descriptor);

        ~PrimitiveFieldSkbGenerator();

        // implements FieldGenerator ---------------------------------------
        void generate_members(google::protobuf::io::Printer *printer) const;

        void generate_members_declares(google::protobuf::io::Printer *printer) const override;

        void generate_move_ctor_define(google::protobuf::io::Printer *printer) const override;
        void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const override;

        void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const override;
        void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const override;
        void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const;

        std::string get_default_value(void) const override;

        std::string do_get_default_value(void) const;
    private:
        bool is_atomic{false};
    };
} // namespace shark
