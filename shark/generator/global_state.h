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

#include <type_traits>
#include <turbo/container/flat_hash_map.h>
#include <turbo/container/flat_hash_set.h>
#include <google/protobuf/descriptor.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/log/logging.h>

namespace shark {

    /// no need lock, always single thread
    struct  GlobalState {

        static GlobalState &instance() {
            static GlobalState instance;
            return instance;
        }

        /// file
        void registry(const google::protobuf::FileDescriptor *file);

        /// message
        void registry(const google::protobuf::Descriptor *descriptor);

        void registry(const google::protobuf::FieldDescriptor *descriptor);

        static std::string cpp_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try = false);

        static std::string complex_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try = false);

        static std::string map_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try = false);

        static bool is_primitive_type(google::protobuf::FieldDescriptor::Type type);

        static bool is_cpp_type(google::protobuf::FieldDescriptor::Type type);

        static::std::string get_ctype(const google::protobuf::FieldDescriptor *descriptor);

        turbo::flat_hash_map<std::string, std::string> pb_to_sk;
        turbo::flat_hash_map<std::string, std::string> sk_to_pb;
        turbo::flat_hash_set<std::string> includes;
        /// extend options
        idl::SharkFileOptions ext_file_options;
        turbo::flat_hash_map<std::string, idl::SharkMessageOptions> ext_message_options;

        turbo::flat_hash_map<std::string, std::string> ns_mapping;

        std::string cnamespace;
        std::string pb_namespace;
    private:
        void process_deps(const google::protobuf::FileDescriptor *file);
    };
}  // namespace shark
