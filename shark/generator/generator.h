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


// Generates C code for a given .proto file.

#pragma once

#include <string>

#include <google/protobuf/compiler/code_generator.h>

#if defined(_WIN32) && defined(PROTOBUF_C_USE_SHARED_LIB)
# define PROTOBUF_C_EXPORT __declspec(dllexport)
#else
# define PROTOBUF_C_EXPORT
#endif

namespace shark {
    // CodeGenerator implementation which generates a C++ source file and
    // header.  If you create your own protocol compiler binary and you want
    // it to support C++ output, you can do so by registering an instance of this
    // CodeGenerator with the CommandLineInterface in your main() function.
    class PROTOBUF_C_EXPORT SharkGenerator : public google::protobuf::compiler::CodeGenerator {
    public:
        SharkGenerator();

        ~SharkGenerator();

        // implements CodeGenerator ----------------------------------------
        bool Generate(const google::protobuf::FileDescriptor *file,
                      const std::string &parameter,
                      google::protobuf::compiler::OutputDirectory *output_directory,
                      std::string *error) const;

#if GOOGLE_PROTOBUF_VERSION >= 5026000
        uint64_t GetSupportedFeatures() const { return 0; }
        google::protobuf::Edition GetMinimumEdition() const { return google::protobuf::Edition::EDITION_PROTO2; }
        google::protobuf::Edition GetMaximumEdition() const { return google::protobuf::Edition::EDITION_PROTO3; }
#endif
    };
} // namespace shark

