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


#include <map>
#include <string>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

namespace shark {
    class ServiceGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit ServiceGenerator(const google::protobuf::ServiceDescriptor *descriptor,
                                  const std::string &dllexport_decl);

        ~ServiceGenerator();

        // Header stuff.
        void GenerateMainHFile(google::protobuf::io::Printer *printer);

        void GenerateVfuncs(google::protobuf::io::Printer *printer);

        void GenerateInitMacros(google::protobuf::io::Printer *printer);

        void GenerateDescriptorDeclarations(google::protobuf::io::Printer *printer);

        void GenerateCallersDeclarations(google::protobuf::io::Printer *printer);

        // Source file stuff.
        void GenerateCFile(google::protobuf::io::Printer *printer);

        void GenerateServiceDescriptor(google::protobuf::io::Printer *printer);

        void GenerateInit(google::protobuf::io::Printer *printer);

        void GenerateCallersImplementations(google::protobuf::io::Printer *printer);

        const google::protobuf::ServiceDescriptor *descriptor_;
        std::map<std::string, std::string> vars_;
    };
} // namespace shark
