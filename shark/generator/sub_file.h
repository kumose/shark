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
#include <vector>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>


namespace shark {
    class SubFileGeneratorBase {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit SubFileGeneratorBase(const google::protobuf::FileDescriptor *file) : _file(file) {
        }

        virtual ~SubFileGeneratorBase() = default;

        /// header region
        virtual void generate_fwd_typedef(google::protobuf::io::Printer *printer) = 0;

        virtual void generate_definition(google::protobuf::io::Printer *printer) = 0;

        virtual void generate_inline_implement(google::protobuf::io::Printer *printer) = 0;


        /// source range
        virtual void generate_static_variable(google::protobuf::io::Printer *printer) = 0;

        virtual void generate_static_functions(google::protobuf::io::Printer *printer) = 0;

        virtual void generate_implement(google::protobuf::io::Printer *printer) = 0;

    protected:
        const google::protobuf::FileDescriptor *_file{nullptr};
    };
} // namespace shark
