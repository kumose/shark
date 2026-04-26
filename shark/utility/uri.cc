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

#include <shark/utility/uri.h>
#include <turbo/strings/ascii.h>
#include <turbo/strings/str_split.h>
#include <turbo/strings/str_replace.h>
#include <turbo/strings/strip.h>
#include <shark/generator/global_state.h>

namespace shark {
    const char *const kKeywordList[] = {
        "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case",
        "catch", "char", "class", "compl", "const", "const_cast", "continue",
        "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
        "explicit", "extern", "false", "float", "for", "friend", "goto", "if",
        "inline", "int", "long", "mutable", "namespace", "new", "not", "not_eq",
        "operator", "or", "or_eq", "private", "protected", "public", "register",
        "reinterpret_cast", "return", "short", "signed", "sizeof", "static",
        "static_cast", "struct", "switch", "template", "this", "throw", "true", "try",
        "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual",
        "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };

    std::set<std::string> MakeKeywordsMap() {
        std::set<std::string> result;
        for (size_t i = 0; i < GOOGLE_ARRAYSIZE(kKeywordList); i++) {
            result.insert(kKeywordList[i]);
        }
        return result;
    }

    std::set<std::string> kKeywords = MakeKeywordsMap();

    std::string varify_field_name(const google::protobuf::FieldDescriptor *field) {
        std::string result = turbo::str_to_lower(field->name());
        if (kKeywords.count(result) > 0) {
            result.append("_");
        }
        return result;
    }

    std::string field_uri(const google::protobuf::FieldDescriptor *field) {
        return field->full_name();
    }

    std::string field_uri_without_namespace(const google::protobuf::FieldDescriptor *field) {
        auto p = field->file()->package();
        auto s = field->full_name();
        return s.substr(p.size());
    }


    std::string message_type(const google::protobuf::Descriptor *md) {
        auto s = md->full_name();
        auto t = turbo::str_replace_all(s, {{".", "::"}});
        return std::string(turbo::strip_prefix(t, GlobalState::instance().pb_namespace_prefix));
    }

    std::string message_type(const google::protobuf::Descriptor *des, const std::string &suffix) {
        auto f = des->file()->package();
        auto mf = des->full_name();
        auto t = std::string(turbo::strip_prefix(mf, f + "."));
        std::vector<std::string_view> sps = turbo::str_split(t, '.');
        std::string result;
        for (size_t i = 0; i < sps.size(); i++) {
            result += turbo::str_cat(sps[i], suffix, "::");
        }
        /// pop "::"
        result.pop_back();
        result.pop_back();

        result = turbo::str_replace_all(f, {{".", "::"}}) + "::" +result;
        return std::string(turbo::strip_prefix(result, GlobalState::instance().pb_namespace_prefix));
    }

    std::string relative_message_type(const google::protobuf::Descriptor *d, const google::protobuf::Descriptor *r) {
        auto dm = message_type(d);
        std::string rm;
        if (r) {
            rm = message_type(r) + "::";
        }
        return std::string(turbo::strip_prefix(dm, rm));
    }

    std::string relative_message_type(const google::protobuf::Descriptor *d, const google::protobuf::Descriptor *r, const std::string &suffix) {
        auto dm = message_type(d, suffix);
        std::string rm;
        if (r) {
            rm = message_type(r, suffix) + "::";
        }
        return std::string(turbo::strip_prefix(dm, rm));
    }

    std::string message_domain_without_namespace(const google::protobuf::FieldDescriptor *field) {
        return message_domain_without_namespace(field->containing_type());
    }

    std::string message_domain_without_namespace(const google::protobuf::Descriptor *md) {
        auto s = md->full_name();
        s = s.substr(md->file()->package().size() + 1);
        return turbo::str_replace_all(s, {{".", "::"}});
    }


    std::string message_domain_without_namespace(const google::protobuf::FieldDescriptor *field,
                                                 const std::string &suffix) {
        return message_domain_without_namespace(field->containing_type(), suffix);
    }

    std::string message_domain_without_namespace(const google::protobuf::Descriptor *md, const std::string &suffix) {
        auto s = md->full_name();
        s = s.substr(md->file()->package().size() + 1);
        std::vector<std::string_view> sps = turbo::str_split(s, '.');
        std::string result;
        for (size_t i = 0; i < sps.size(); i++) {
            result += turbo::str_cat(sps[i], suffix, "::");
        }
        /// pop "::"
        result.pop_back();
        result.pop_back();
        return result;
    }

    std::string enum_type(const google::protobuf::FieldDescriptor *field) {
        return enum_type(field->enum_type());
    }

    std::string enum_type(const google::protobuf::EnumDescriptor *field) {
        return turbo::str_replace_all(field->full_name(), {{".", "::"}});
    }
} // namespace shark
