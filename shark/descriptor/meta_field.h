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

#include <shark/descriptor/meta_map.h>

namespace shark {
    class MetaFieldDescriptorGenerator : public FieldDescriptorGenerator {
    public:
        explicit MetaFieldDescriptorGenerator(const google::protobuf::FieldDescriptor *descriptor,
                                              const FieldMetaMap *map, std::string path);

        ~MetaFieldDescriptorGenerator();


        void generate_meta_declares(google::protobuf::io::Printer *printer) const override {
        }

        void generate_meta_definition(google::protobuf::io::Printer *printer) const override {
        }

        void generate_meta_definition_inline(google::protobuf::io::Printer *printer) const override;

    protected:
        void generate_meta_static_defines(google::protobuf::io::Printer *printer) const;

        void generate_meta_function_defines(google::protobuf::io::Printer *printer) const;

        std::vector<std::string_view> lvs;
        std::vector<int> lvs2;
        std::vector<FieldMeta *> meta_path;
        FieldMeta *_meta{nullptr};
    };
} // namespace shark
