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

#include <shark/ast/cpp_entity.h>
#include <shark/ast/cpp_expression.h>

namespace cppast {
    class CppStaticAssert : public CppEntity {
    public:
        static CppEntityKind kind() noexcept;

        /// \returns A newly created `static_assert()` entity.
        /// \notes It will not be registered as nothing can refer to it.
        static std::unique_ptr<CppStaticAssert> build(std::unique_ptr<CppExpression> expr,
                                                      std::string msg) {
            return std::unique_ptr<CppStaticAssert>(
                new CppStaticAssert(std::move(expr), std::move(msg)));
        }

        /// \returns A reference to the [cppast::CppExpression]() that is being asserted.
        const CppExpression &expression() const noexcept {
            return *expr_;
        }

        /// \returns A reference to the message of the assertion.
        const std::string &message() const noexcept {
            return msg_;
        }

    private:
        CppStaticAssert(std::unique_ptr<CppExpression> expr, std::string msg)
            : CppEntity(""), expr_(std::move(expr)), msg_(std::move(msg)) {
        }

        CppEntityKind do_get_entity_kind() const noexcept override;

        std::unique_ptr<CppExpression> expr_;
        std::string msg_;
    };
} // namespace cppast
