// SPDX-License-Identifier: BSD-3-Clause
// Based on Google Protobuf (https://github.com/protocolbuffers/protobuf) and protobuf-c
// (https://github.com/protobuf-c/protobuf-c)
// Copyright 2008 Google Inc., 2008-2025 protobuf-c authors. Modifications for C++ generation.
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

#include <shark/generator/enum.h>
#include <shark/generator/extension.h>
#include <shark/generator/field.h>
#include <shark/generator/message.h>
#include <shark/generator/service.h>

namespace shark {
    class FileGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit FileGenerator(const google::protobuf::FileDescriptor *file,
                               const std::string &dllexport_decl);

        ~FileGenerator();

        void GenerateHeader(google::protobuf::io::Printer *printer);

        void GenerateSource(google::protobuf::io::Printer *printer);

    private:
        const google::protobuf::FileDescriptor *file_;
        bool  _have_one_of{false};

        std::unique_ptr<std::unique_ptr<MessageGenerator>[]> message_generators_;
        std::unique_ptr<std::unique_ptr<EnumGenerator>[]> enum_generators_;
        std::unique_ptr<std::unique_ptr<ServiceGenerator>[]> service_generators_;
        std::unique_ptr<std::unique_ptr<ExtensionGenerator>[]> extension_generators_;
    };
} // namespace shark
