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
#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/descriptor/meta_map.h>
#include <shark/generator/message.h>

namespace shark {
    class MetaMessageGenerator : public MessageGeneratorBase {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit MetaMessageGenerator(const google::protobuf::Descriptor *descriptor,
                                      const std::string &dllexport_decl);

        ~MetaMessageGenerator() override = default;

        /// header region
        void generate_fwd_typedef(google::protobuf::io::Printer *printer) override;

        void generate_definition(google::protobuf::io::Printer *printer) override;

        void generate_inline_implement(google::protobuf::io::Printer *printer) override {

        }


        /// source range
        void generate_static_variable(google::protobuf::io::Printer *printer) override {

        }

        void generate_static_functions(google::protobuf::io::Printer *printer) override {

        }

        void generate_implement(google::protobuf::io::Printer *printer) override {

        }

    private:
        FieldMetaMap _field_generators;
    };
} // namespace shark
