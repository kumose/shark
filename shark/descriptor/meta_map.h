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
#include <shark/generator/field_descriptor.h>

namespace shark {
    struct FieldMeta {
        int index{0};
        int data_index{0};
        const google::protobuf::Descriptor *root{nullptr};
        std::string path;
        const google::protobuf::FieldDescriptor *field{nullptr};
        std::vector<int> column;
        bool repeated{false};
        std::string cpp_type;
        bool is_map{false};
        bool is_any{false};
    };
    class MetaFieldDescriptorGenerator;
    // Convenience class which constructs FieldGenerators for a Descriptor.
    class FieldMetaMap {
    public:
        explicit FieldMetaMap(const google::protobuf::Descriptor *descriptor);

        ~FieldMetaMap() = default;

        const MetaFieldDescriptorGenerator &get(const google::protobuf::FieldDescriptor *field) const;

        const std::vector<std::unique_ptr<FieldMeta> > &metas() const {
            return field_metas;
        }

        const turbo::flat_hash_map<std::string, FieldMeta *> &meta_map() const {
            return field_meta_map;
        }

    private:
        const google::protobuf::Descriptor *descriptor_;
        std::vector<std::unique_ptr<MetaFieldDescriptorGenerator> > field_generators_;

        int _index{0};
        int _data_index{0};


        turbo::flat_hash_map<std::string, FieldMeta *> field_meta_map;

        std::vector<std::unique_ptr<FieldMeta> > field_metas;

        void process_message();

        static std::unique_ptr<MetaFieldDescriptorGenerator> make_generator(const google::protobuf::FieldDescriptor *field,const FieldMetaMap* map,std::string path);
    };
} // namespace shark
