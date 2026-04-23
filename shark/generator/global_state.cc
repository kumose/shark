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

#include <shark/generator/global_state.h>
#include <shark/utility/type.h>
#include <turbo/strings/match.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/match.h>
#include <turbo/strings/str_cat.h>

namespace shark {

    void GlobalState::registry(const google::protobuf::FileDescriptor *file) {
        g_file = file;
        ext_file_options = file->options().GetExtension(idl::shark_file);
        pb_namespace = turbo::str_replace_all(file->package(), {{".", "::"}});
        pb_namespace_prefix = pb_namespace + "::";
        if (!ext_file_options.has_runtime_namespace() && ext_file_options.runtime_namespace().empty()) {
            KLOG(FATAL) << "must spefic runtime_namespace";
        }
        cnamespace = ext_file_options.runtime_namespace();
        ns_mapping[pb_namespace] = cnamespace;
        for (int i = 0; i < file->dependency_count(); ++i) {
            const auto *dep = file->dependency(i);
            process_deps(dep);
        }
        if (ext_file_options.cpp_includes_size()) {
            for (int i = 0; i < ext_file_options.cpp_includes_size(); ++i) {
                includes.insert(turbo::str_cat("#include <", ext_file_options.cpp_includes(i), ">"));
            }
        }

    }


    void GlobalState::process_deps(const google::protobuf::FileDescriptor *file) {
        std::vector<const google::protobuf::FileDescriptor *> files;
        std::deque<const google::protobuf::FileDescriptor *> tmp;
        files.push_back(file);
        tmp.push_back(file);
        while (!tmp.empty()) {
            auto f = tmp.front();
            tmp.pop_front();
            for (int i = 0; i < f->dependency_count(); ++i) {
                const auto *dep = f->dependency(i);
                files.push_back(dep);
                tmp.push_back(dep);
            }
        }
        for (int i = 0; i < files.size(); ++i) {
            auto file_options = files[i]->options().GetExtension(idl::shark_file);
            auto pb_n = turbo::str_replace_all(files[i]->package(), {{".", "::"}});
            if (turbo::starts_with(files[i]->package(), "google.protobuf")) {
                continue;
            }
            if (!file_options.has_runtime_namespace() || file_options.runtime_namespace().empty()) {
                KLOG(FATAL) << "must spefic runtime_namespace:" << file->package();
            }
            auto cn = file_options.runtime_namespace();
            ns_mapping[pb_n] = cn;
            for (int j = 0; j < files[i]->message_type_count(); j++) {
                registry(files[i]->message_type(j));
            }
        }
    }

    void GlobalState::registry(const google::protobuf::Descriptor *descriptor) {
        auto ext_options = descriptor->options().GetExtension(idl::shark_message);
        GlobalState::instance().ext_message_options[descriptor->full_name()] = ext_options;
        auto ctype = turbo::str_replace_all(descriptor->full_name(), {{".", "::"}});
        pb_to_sk[descriptor->full_name()] = ctype;
        sk_to_pb[ctype] = descriptor->full_name();
    }

    void GlobalState::registry(const google::protobuf::FieldDescriptor *descriptor) {
        auto ext_options = descriptor->options().GetExtension(idl::shark_field);
        if (ext_options.is_atomic()) {
            includes.insert("#include <atomic>");
        }
        std::string rp_type;
        if (descriptor->is_repeated()) {
            rp_type = "std::vector";
            if (!instance().ext_file_options.sk_vector().empty()) {
                rp_type = instance().ext_file_options.sk_vector();
            }
        }

        static turbo::flat_hash_map<std::string, std::string> rep_header = {
            {"std::vector", "vector"},
            {"std::deque", "deque"},
            {"std::set", "set"},
            {"std::unordered_set", "unordered_set"},
            {"turbo::flat_hash_set", "turbo/container/flat_hash_set.h"},
            {"turbo::btree_set", "turbo/container/btree_set.h"},
            {"std::list", "list"},
        };
        if (!rp_type.empty()) {
            auto it = rep_header.find(rp_type);
            if (it != rep_header.end()) {
                instance().includes.insert(turbo::str_cat("#include <", it->second, ">"));
            } else {
                KLOG(WARNING) << "can not found repeat container type, include it your self type:" << rp_type;
            }
        }

        static turbo::flat_hash_map<std::string, std::string> map_header = {
            {"std::map", "map"},
            {"std::unordered_map", "unordered_map"},
            {"turbo::flat_hash_map", "turbo/container/flat_hash_map.h"},
            {"turbo::btree_map", "turbo/container/btree_map.h"},
        };

    }

} // namespace shark
