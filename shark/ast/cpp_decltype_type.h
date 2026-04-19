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
    /// A [cppast::CppType]() that isn't given but taken from an expression.
    class CppDecltypeType final : public CppType {
    public:
        /// \returns A newly created `decltype` type.
        static std::unique_ptr<CppDecltypeType> build(std::unique_ptr<CppExpression> expr) {
            return std::unique_ptr<CppDecltypeType>(new CppDecltypeType(std::move(expr)));
        }

        /// \returns A reference to the expression given.
        const CppExpression &expression() const noexcept {
            return *expr_;
        }

    private:
        CppDecltypeType(std::unique_ptr<CppExpression> expr) : expr_(std::move(expr)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::decltype_t;
        }

        std::unique_ptr<CppExpression> expr_;
    };

    /// A [cppast::CppType]() that isn't given but deduced using the `decltype` rules.
    class CppDecltypeAutoType final : public CppType {
    public:
        /// \returns A newly created `auto` type.
        static std::unique_ptr<CppDecltypeAutoType> build() {
            return std::unique_ptr<CppDecltypeAutoType>(new CppDecltypeAutoType);
        }

    private:
        CppDecltypeAutoType() = default;

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::decltype_auto_t;
        }
    };
} // namespace cppast
