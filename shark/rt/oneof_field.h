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
#include <string_view>
#include <shark/generator/field_map.h>

#include <shark/utility/uri.h>


namespace shark {
    class OneofFieldGenerator {
    public:
        explicit OneofFieldGenerator(const google::protobuf::Descriptor *mdes);

        ~OneofFieldGenerator();

        void generate_enum_def(google::protobuf::io::Printer *printer) const;

        // implements FieldGenerator ---------------------------------------
        void generate_members(google::protobuf::io::Printer *printer) const;

        void generate_members_declares(google::protobuf::io::Printer *printer) const;

        void generate_ctor_define(google::protobuf::io::Printer *printer) const;

        void generate_move_ctor_define(google::protobuf::io::Printer *printer) const;

        void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const;

        void generate_dtor_define(google::protobuf::io::Printer *printer) const;

        void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const;


        void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const;

        void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const;

        void GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const;


        std::string do_get_default_value(void) const;

        std::string get_default_value(void) const;

    private:
        const google::protobuf::Descriptor *mdes_;
        std::map<std::string, std::string> _variable;
    };
} // namespace shark
