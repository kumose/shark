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


#include <shark/generator/sub_file.h>
#include <shark/rt/enum.h>
#include <shark/rt/extension.h>
#include <shark/generator/field_map.h>
#include <shark/generator/file.h>
#include <shark/rt/message.h>
#include <shark/rt/service.h>
#include <shark/descriptor/meta_message.h>

namespace shark {
    class DescriptorFileGenerator : public SubFileGeneratorBase {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit DescriptorFileGenerator(const google::protobuf::FileDescriptor *file,
                               const std::string &dllexport_decl);

        ~DescriptorFileGenerator() override = default;

        /// header region
        void generate_fwd_typedef(google::protobuf::io::Printer *printer) override;

        void generate_definition(google::protobuf::io::Printer *printer) override;

        void generate_inline_implement(google::protobuf::io::Printer *printer) override;


        /// source range
        void generate_static_variable(google::protobuf::io::Printer *printer) override;

        void generate_static_functions(google::protobuf::io::Printer *printer) override;

        void generate_implement(google::protobuf::io::Printer *printer) override;

    protected:
        bool  _have_one_of{false};

        std::vector<std::unique_ptr<MessageGeneratorBase>> message_generators_;
    };
} // namespace shark
