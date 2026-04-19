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

#include <shark/ast/cpp_expression.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// Additional base class for all [cppast::CppEntity]() modelling some kind of variable.
    ///
    /// Examples are [cppast::CppVariable]() or [cppast::CppFunctionParameter](),
    /// or anything that is name/type/default-value triple.
    class CppVariableBase {
    public:
        /// \returns A reference to the [cppast::CppType]() of the variable.
        const CppType &type() const noexcept {
            return *type_;
        }

        /// \returns A [ts::optional_ref]() to the [cppast::CppExpression]() that is the default value.
        type_safe::optional_ref<const CppExpression> default_value() const noexcept {
            return type_safe::opt_ref(default_.get());
        }

    protected:
        CppVariableBase(std::unique_ptr<CppType> type, std::unique_ptr<CppExpression> def)
            : type_(std::move(type)), default_(std::move(def)) {
        }

        ~CppVariableBase() noexcept = default;

    private:
        std::unique_ptr<CppType> type_;
        std::unique_ptr<CppExpression> default_;
    };
} // namespace cppast
