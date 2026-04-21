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

#include <shark/meta/global_state.h>
#include <turbo/strings/match.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/match.h>
#include <turbo/strings/str_cat.h>

namespace shark {
    std::string GlobalState::cpp_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try) {
        auto opt = descriptor->options();
        auto ext_opt = opt.GetExtension(idl::shark_field);
        auto opt_type = ext_opt.ctype();
        if (!opt_type.empty()) {
            return opt_type;
        }
        switch (descriptor->type()) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
                return "int32_t";
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
                return "int64_t";
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
                return "uint32_t";
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
                return "uint64_t";
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                return "float";
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                return "double";
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
                return "bool";
            case google::protobuf::FieldDescriptor::TYPE_STRING:
                return "std::string";
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return "std::vector<uint8_t>";
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                if (!is_try) {
                    KLOG(FATAL) << "not a primitive type:" << static_cast<int>(descriptor->type());
                }
                return "";

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    bool GlobalState::is_primitive_type(google::protobuf::FieldDescriptor::Type type) {
        switch (type) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
                return true;
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_STRING:
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                return false;

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    bool GlobalState::is_cpp_type(google::protobuf::FieldDescriptor::Type type) {
        switch (type) {
            case google::protobuf::FieldDescriptor::TYPE_SINT32:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            case google::protobuf::FieldDescriptor::TYPE_INT32:
            case google::protobuf::FieldDescriptor::TYPE_SINT64:
            case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            case google::protobuf::FieldDescriptor::TYPE_INT64:
            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            case google::protobuf::FieldDescriptor::TYPE_BOOL:
            case google::protobuf::FieldDescriptor::TYPE_STRING:
            case google::protobuf::FieldDescriptor::TYPE_BYTES:
                return true;
            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            case google::protobuf::FieldDescriptor::TYPE_GROUP:
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            default:
                return false;

                // No default because we want the compiler to complain if any new
                // types are added.
        }
    }

    void GlobalState::registry(const google::protobuf::FileDescriptor *file) {
        ext_file_options = file->options().GetExtension(idl::shark_file);
        pb_namespace = turbo::str_replace_all(file->package(), {{".", "::"}});
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
        for (auto i  = 0; i <file->message_type_count(); i++) {
            process_message(file->message_type(i));
        }
    }

    int cnt = 0;
    void GlobalState::process_message(const google::protobuf::Descriptor *file) {
        for (auto i = 0; i < file->field_count(); ++i ) {
            auto f = file->field(i);
            KLOG(INFO)<<"id:"<<cnt++<<" field:"<<f->full_name()<<" name:"<<f->name();
        }
        KLOG(INFO)<<"--------------------------------------";
        for (int i = 0; i < file->nested_type_count(); ++i ) {
            process_message(file->nested_type(i));
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

        if (descriptor->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE &&
            descriptor->message_type()->options().map_entry()) {
            auto type = complex_type(descriptor, true);
            if (!type.empty()) {
                auto pos = type.find('<');
                if (pos != std::string::npos) {
                    type = type.substr(0, pos);
                }
            }
            if (!type.empty()) {
                auto it = map_header.find(type);
                if (it != rep_header.end()) {
                    instance().includes.insert(turbo::str_cat("#include <", it->second, ">"));
                } else {
                    KLOG(WARNING) << "can not found repeat container type, include it your self type:" << type;
                }
            }
        }
    }

    std::string GlobalState::complex_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try) {
        if (descriptor->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE &&
            descriptor->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) {
            if (is_try) {
                return "";
            }
            KLOG(FATAL) << "must spefic enum or message type";
        }
        if (descriptor->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE &&
            descriptor->message_type()->options().map_entry()) {
            return map_type(descriptor, is_try);
        }
        std::string f;
        if (descriptor->type() == google::protobuf::FieldDescriptor::TYPE_ENUM) {
            f = descriptor->enum_type()->full_name();
        } else if (descriptor->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
            f = descriptor->message_type()->full_name();
        }
        auto it = instance().pb_to_sk.find(f);
        if (it == instance().pb_to_sk.end()) {
            if (!is_try) {
                KLOG(FATAL) << "unkown type:" << descriptor->full_name();
            }
            return "";
        }
        auto find = it->second;
        for (auto &it: instance().ns_mapping) {
            if (turbo::starts_with(find, it.first)) {
                return turbo::str_cat(it.second, find.substr(it.first.size()));
            }
        }

        if (!is_try) {
            KLOG(FATAL) << "unkown type:" << descriptor->full_name();
        }
        return "";
    }

    std::string GlobalState::map_type(const google::protobuf::FieldDescriptor *descriptor, bool is_try) {
        // Get the map entry message
        const auto *entry = descriptor->message_type();
        const auto *key_field = entry->field(0);
        const auto *value_field = entry->field(1);

        // Recursively get C++ types for key and value
        std::string key_type = get_ctype(key_field);
        std::string value_type = get_ctype(value_field);
        if (key_type.empty() || value_type.empty()) {
            if (!is_try)
                KLOG(FATAL) << "failed to get key/value type for map: " << descriptor->full_name();
            return "";
        }

        // Obtain sk_map_type template from the file that defines this map
        const auto *file = descriptor->file();
        auto file_opts = file->options().GetExtension(idl::shark_file);
        std::string map_template = "std::unordered_map";
        if (file_opts.has_sk_map_type()) {
            map_template = file_opts.sk_map_type();
        }

        // Replace placeholders
        size_t pos = map_template.find("<");
        std::string mtype = map_template;
        if (pos != std::string::npos) {
            mtype = map_template.substr(0, pos);
        }

        return turbo::str_cat(mtype, "<", key_type, ",", value_type, ">");
    }

    std::string GlobalState::get_ctype(const google::protobuf::FieldDescriptor *descriptor) {
        auto typ = cpp_type(descriptor, true);
        auto opt = descriptor->options().GetExtension(idl::shark_field);
        bool is_atomic =opt.is_atomic();
        auto com_typ = complex_type(descriptor, true);
        std::string find = typ;
        if (!com_typ.empty()) {
            find = com_typ;
        }
        if (find.empty()) {
            KLOG(FATAL) << "must spefic enum or message type";
        }

        if (is_atomic) {
            if (descriptor->is_repeated() || !is_primitive_type(descriptor->type())) {
                KLOG(FATAL) << "repeat or complexed type can not be atomic";
            }
        }
        return find;
    }
} // namespace shark
