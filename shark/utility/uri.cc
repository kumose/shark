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


    std::string message_domain(const google::protobuf::FieldDescriptor *field) {
        return message_domain(field->containing_type());
    }

    std::string message_domain_without_namespace(const google::protobuf::FieldDescriptor *field) {
        return message_domain_without_namespace(field->containing_type());
    }

    std::string message_domain(const google::protobuf::Descriptor *md) {
        auto s = md->full_name();
        return turbo::str_replace_all(s, {{".", "::"}});
    }

    std::string message_domain_without_namespace(const google::protobuf::Descriptor *md) {
        KLOG(INFO)<<1;
        auto s = md->full_name();
        KLOG(INFO)<<2;
        s = s.substr(md->file()->package().size() + 1);
        KLOG(INFO)<<3;
        return turbo::str_replace_all(s, {{".", "::"}});
    }


}  // namespace shark
