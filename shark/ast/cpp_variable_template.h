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
#include <shark/ast/cpp_variable.h>

namespace cppast {
    /// A [cppast::CppEntity]() modelling a C++ alias template.
    class CppVariableTemplate final : public CppTemplate {
    public:
        static CppEntityKind kind() noexcept;

        /// Builder for [cppast::CppVariableTemplate]().
        class builder : public basic_builder<CppVariableTemplate, CppVariable> {
        public:
            using basic_builder::basic_builder;
        };

        /// \returns A reference to the type variable that is being templated.
        const CppVariable &variable() const noexcept {
            return static_cast<const CppVariable &>(*begin());
        }

    private:
        CppVariableTemplate(std::unique_ptr<CppVariable> variable)
            : CppTemplate(std::unique_ptr<CppEntity>(variable.release())) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        friend basic_builder<CppVariableTemplate, CppVariable>;
    };
} // namespace cppast
