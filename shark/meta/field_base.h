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

#include <memory>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/utility/status.h>
#include <shark/utility/helpers.h>
#include <shark/meta/global_state.h>

namespace shark {
    class FieldGeneratorBase {
    public:
        explicit FieldGeneratorBase(const google::protobuf::FieldDescriptor *descriptor);

        void initialize();


        virtual ~FieldGeneratorBase() = default;
        ///////////////////////////////
        /// constructor and destructor calls
        virtual void generate_move_ctor_define(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_meta_declares(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_meta_definition(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_members_declares(google::protobuf::io::Printer *printer) const = 0;


        // Generate definitions to be included in the structure.
        virtual void generate_members(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const = 0;

        virtual std::string get_default_value() const = 0;



    protected:
        virtual void do_initialize() {

        }

        const google::protobuf::FieldDescriptor *descriptor_;
        std::map<std::string, std::string> variables_;
        idl::SharkFieldOptions _ext_option;
        google::protobuf::FieldOptions _option;
    };
} // namespace shark
