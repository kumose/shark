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

#include <shark/ast/cpp_template.h>
#include <shark/ast/cpp_type_alias.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a C++ alias template.
    class CppAliasTemplate final : public CppTemplate {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppAliasTemplate]().
        class builder : public basic_builder<CppAliasTemplate, CppTypeAlias> {
        public:
            using basic_builder::basic_builder;
        };

        /// \returns A reference to the type alias that is being templated.
        const CppTypeAlias &type_alias() const noexcept {
            return static_cast<const CppTypeAlias &>(*begin());
        }

    private:
        CppAliasTemplate(std::unique_ptr<CppTypeAlias> alias)
            : CppTemplate(std::unique_ptr<CppEntity>(alias.release())) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        friend basic_builder<CppAliasTemplate, CppTypeAlias>;
    };
} // namespace cppast
