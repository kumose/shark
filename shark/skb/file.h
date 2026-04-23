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

#include <shark/builder/enum.h>
#include <shark/builder/extension.h>
#include <shark/generator/field_map.h>
#include <shark/builder/message.h>
#include <shark/builder/service.h>

#include <shark/view/enum.h>
#include <shark/view/extension.h>
#include <shark/generator/field_map.h>
#include <shark/view/message.h>
#include <shark/view/service.h>

namespace shark {
    class FileSkbGenerator {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit FileSkbGenerator(const google::protobuf::FileDescriptor *file,
                               const std::string &dllexport_decl);

        ~FileSkbGenerator();

        void GenerateHeader(google::protobuf::io::Printer *printer);

        void GenerateSource(google::protobuf::io::Printer *printer);

    private:
        const google::protobuf::FileDescriptor *file_;
        bool  _have_one_of{false};

        std::vector<std::unique_ptr<MessageSkbGenerator>> message_generators_;
        std::vector<std::unique_ptr<EnumSkbGenerator>> enum_generators_;
        std::vector<std::unique_ptr<ServiceSkbGenerator>> service_generators_;
        std::vector<std::unique_ptr<ExtensionSkbGenerator>> extension_generators_;

        std::vector<std::unique_ptr<MessageViewGenerator>> message_view_generators_;
        std::vector<std::unique_ptr<EnumViewGenerator>> enum_view_generators_;
        std::vector<std::unique_ptr<ServiceViewGenerator>> service_view_generators_;
        std::vector<std::unique_ptr<ExtensionViewGenerator>> extension_view_generators_;
    };
} // namespace shark
