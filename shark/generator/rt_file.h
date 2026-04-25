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
#include <shark/generator/field_map.h>
#include <shark/generator/file.h>
#include <shark/rt/file.h>


namespace shark {
    class FileRtGenerator : public FileGeneratorBase {
    public:
        // See generator.cc for the meaning of dllexport_decl.
        explicit FileRtGenerator(const google::protobuf::FileDescriptor *file,
                               const std::string &dllexport_decl);

        ~FileRtGenerator() override;

        void generate_header(google::protobuf::io::Printer *printer);

        void generate_source(google::protobuf::io::Printer *printer);

    private:
        bool  _have_one_of{false};
        std::unique_ptr<SubFileGeneratorBase> _rt_file;
    };
} // namespace shark
