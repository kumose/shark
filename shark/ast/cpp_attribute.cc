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


#include <shark/ast/cpp_attribute.h>
#include <algorithm>
#include <shark/ast/cpp_entity.h>

using namespace cppast;

namespace {
    const char *get_attribute_name(CppAttributeKind kind) noexcept {
        switch (kind) {
            case CppAttributeKind::alignas_:
                return "alignas";
            case CppAttributeKind::carries_dependency:
                return "carries_dependency";
            case CppAttributeKind::deprecated:
                return "deprecated";
            case CppAttributeKind::fallthrough:
                return "fallthrough";
            case CppAttributeKind::maybe_unused:
                return "maybe_unused";
            case CppAttributeKind::nodiscard:
                return "nodiscard";
            case CppAttributeKind::noreturn:
                return "noreturn";

            case CppAttributeKind::unknown:
                return "unknown";
        }

        return "<error>";
    }
} // namespace

CppAttribute::CppAttribute(CppAttributeKind kind,
                           type_safe::optional<CppTokenString> arguments)
    : CppAttribute(type_safe::nullopt, get_attribute_name(kind), std::move(arguments), false) {
    kind_ = kind;
}

type_safe::optional_ref<const CppAttribute> cppast::has_attribute(
    const cpp_attribute_list &attributes, const std::string &name) {
    auto iter
            = std::find_if(attributes.begin(), attributes.end(), [&](const CppAttribute &attribute) {
                if (attribute.scope())
                    return attribute.scope().value() + "::" + attribute.name() == name;
                else
                    return attribute.name() == name;
            });

    if (iter == attributes.end())
        return nullptr;
    else
        return type_safe::ref(*iter);
}

type_safe::optional_ref<const CppAttribute> cppast::has_attribute(
    const cpp_attribute_list &attributes, CppAttributeKind kind) {
    auto iter
            = std::find_if(attributes.begin(), attributes.end(),
                           [&](const CppAttribute &attribute) { return attribute.kind() == kind; });

    if (iter == attributes.end())
        return nullptr;
    else
        return type_safe::ref(*iter);
}

type_safe::optional_ref<const CppAttribute> cppast::has_attribute(const CppEntity &e,
                                                                  const std::string &name) {
    return has_attribute(e.attributes(), name);
}

type_safe::optional_ref<const CppAttribute> cppast::has_attribute(const CppEntity &e,
                                                                  CppAttributeKind kind) {
    return has_attribute(e.attributes(), kind);
}
