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
    /// An array of a [cppast::CppType]().
    class CppArrayType final : public CppType {
    public:
        /// \returns A newly created array.
        /// \notes `size` may be `nullptr`.
        static std::unique_ptr<CppArrayType> build(std::unique_ptr<CppType> type,
                                                     std::unique_ptr<CppExpression> size) {
            return std::unique_ptr<CppArrayType>(
                new CppArrayType(std::move(type), std::move(size)));
        }

        /// \returns A reference to the value [cppast::CppType]().
        const CppType &value_type() const noexcept {
            return *type_;
        }

        /// \returns An optional reference to the [cppast::CppExpression]() that is the size of the
        /// array. \notes An unsized array - `T[]` - does not have a size.
        type_safe::optional_ref<const CppExpression> size() const noexcept {
            return type_safe::opt_cref(size_.get());
        }

    private:
        CppArrayType(std::unique_ptr<CppType> type, std::unique_ptr<CppExpression> size)
            : type_(std::move(type)), size_(std::move(size)) {
        }

        CppTypeKind do_get_kind() const noexcept override {
            return CppTypeKind::array_t;
        }

        std::unique_ptr<CppType> type_;
        std::unique_ptr<CppExpression> size_;
    };
} // namespace cppast
