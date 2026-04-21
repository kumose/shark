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

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/utility/status.h>
#include <shark/utility/helpers.h>
#include <shark/meta/global_state.h>
#include <shark/meta/field_meta.h>
namespace shark {

    // Convenience class which constructs FieldGenerators for a Descriptor.
    class FieldGeneratorMap {
    public:
        explicit FieldGeneratorMap(const google::protobuf::Descriptor *descriptor);

        ~FieldGeneratorMap();

        const FieldGeneratorBase &get(const google::protobuf::FieldDescriptor *field) const;

    private:
        const google::protobuf::Descriptor *descriptor_;
        std::vector<std::unique_ptr<FieldGeneratorBase>> field_generators_;

        static std::unique_ptr<FieldGeneratorBase> MakeGenerator(const google::protobuf::FieldDescriptor *field);
    };
} // namespace shark
