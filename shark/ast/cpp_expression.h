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

#include <atomic>
#include <memory>

#include <shark/ast/cpp_token.h>
#include <shark/ast/cpp_type.h>

namespace cppast {
    /// The kind of a [cppast::CppExpression]().
    enum class CppExpressionKind {
        literal_t,

        unexposed_t,
    };

    /// Base class for all C++ expressions.
    class CppExpression {
    public:
        CppExpression(const CppExpression &) = delete;

        CppExpression &operator=(const CppExpression &) = delete;

        virtual ~CppExpression() noexcept = default;

        /// \returns The [cppast::CppExpressionKind]().
        CppExpressionKind kind() const noexcept {
            return do_get_kind();
        }

        /// \returns The type of the expression.
        const CppType &type() const noexcept {
            return *type_;
        }

        /// \returns The specified user data.
        void *user_data() const noexcept {
            return user_data_.load();
        }

        /// \effects Sets some kind of user data.
        ///
        /// User data is just some kind of pointer, there are no requirements.
        /// The class will do no lifetime management.
        ///
        /// User data is useful if you need to store additional data for an entity without the need to
        /// maintain a registry.
        void set_user_data(void *data) const noexcept {
            user_data_ = data;
        }

    protected:
        /// \effects Creates it given the type.
        /// \requires The type must not be `nullptr`.
        CppExpression(std::unique_ptr<CppType> type) : type_(std::move(type)), user_data_(nullptr) {
            DEBUG_ASSERT(type_ != nullptr, detail::precondition_error_handler{});
        }

    private:
        /// \returns The [cppast::CppExpressionKind]().
        virtual CppExpressionKind do_get_kind() const noexcept = 0;

        std::unique_ptr<CppType> type_;
        mutable std::atomic<void *> user_data_;
    };

    /// An unexposed [cppast::CppExpression]().
    ///
    /// There is no further information than a string available.
    class CppUnexposedExpression final : public CppExpression {
    public:
        /// \returns A newly created unexposed expression.
        static std::unique_ptr<CppUnexposedExpression> build(std::unique_ptr<CppType> type,
                                                               CppTokenString str) {
            return std::unique_ptr<CppUnexposedExpression>(
                new CppUnexposedExpression(std::move(type), std::move(str)));
        }

        /// \returns The expression as a string.
        const CppTokenString &expression() const noexcept {
            return str_;
        }

    private:
        CppUnexposedExpression(std::unique_ptr<CppType> type, CppTokenString str)
            : CppExpression(std::move(type)), str_(std::move(str)) {
        }

        CppExpressionKind do_get_kind() const noexcept override {
            return CppExpressionKind::unexposed_t;
        }

        CppTokenString str_;
    };

    /// A [cppast::CppExpression]() that is a literal.
    class CppLiteralExpression final : public CppExpression {
    public:
        /// \returns A newly created literal expression.
        static std::unique_ptr<CppLiteralExpression> build(std::unique_ptr<CppType> type,
                                                             std::string value) {
            return std::unique_ptr<CppLiteralExpression>(
                new CppLiteralExpression(std::move(type), std::move(value)));
        }

        /// \returns The value of the literal, as string.
        const std::string &value() const noexcept {
            return value_;
        }

    private:
        CppLiteralExpression(std::unique_ptr<CppType> type, std::string value)
            : CppExpression(std::move(type)), value_(std::move(value)) {
        }

        CppExpressionKind do_get_kind() const noexcept override {
            return CppExpressionKind::literal_t;
        }

        std::string value_;
    };

    /// \exclude
    namespace detail {
        void write_expression(CodeGenerator::output &output, const CppExpression &expr);
    } // namespace detail
} // namespace cppast
